/* some changes in sub_wave.c required by wave_comparison.c */ 

short int circletop[NMAXCIRCLES];  /* set to 1 if circle is in top half */


void init_circle_config_half(int pattern, int top)
/* initialise the arrays circlex, circley, circlerad and circleactive */
/* for billiard shape D_CIRCLES */
{
    int i, j, k, n, ncirc0, n_p_active, ncandidates=5000, naccepted, nnew; 
    double dx, dy, p, phi, r, r0, ra[5], sa[5], height, x, y = 0.0, gamma, ymean, ytop, ybottom, dpoisson = 3.05*MU;
    short int active_poisson[NMAXCIRCLES], far;
   
    ymean = 0.5*(YMIN + YMAX);
    switch (pattern) {
        case (C_SQUARE):
        {
            dy = (YMAX - YMIN)/((double)NGRIDY);
            for (i = 0; i < NGRIDX; i++)
                for (j = 0; j < NGRIDY/2; j++)
                {
                    printf("i = %i, j = %i, n = %i\n", i, j, n);
                    n = ncircles + NGRIDY*i/2 + j;
                    circlex[n] = ((double)(i-NGRIDX/2) + 0.5)*dy;
                    y = ((double)j + 0.5)*dy;
                    if (top) circley[n] = ymean + y;
                    else circley[n] = ymean - y;
                    circlerad[n] = MU;
                    circleactive[n] = 1;
                    circletop[n] = top;
                }
            ncircles += NGRIDX*NGRIDY/2;
            break;
        }
        case (C_HEX):
        {
            dy = (YMAX - YMIN)/((double)NGRIDY);
            dx = dy*0.5*sqrt(3.0);
            for (i = 0; i < NGRIDX; i++)
                for (j = 0; j < NGRIDY/2+1; j++)
                {
                    n = ncircles + (NGRIDY+1)*i/2 + j;
                    circlex[n] = ((double)(i-NGRIDX/2))*dy;
                    y = ((double)j - 0.5)*dy;
                    if ((i+NGRIDX)%2 == 1) y += 0.5*dy;
                    if (top) circley[n] = ymean + 0.5*dy + y;
                    else circley[n] = ymean - 0.5*dy - y;
                    circlerad[n] = MU;
                    circleactive[n] = 1;
                    circletop[n] = top;
                }
            ncircles += NGRIDX*(NGRIDY+1)/2;
            break;
        }
        case (C_RAND_DISPLACED):
        {
            dy = (YMAX - YMIN)/((double)NGRIDY);
            for (i = 0; i < NGRIDX; i++)
                for (j = 0; j < NGRIDY/2; j++)
                {
                    n = ncircles + NGRIDY*i/2 + j;
                    circlex[n] = ((double)(i-NGRIDX/2) + 0.5*((double)rand()/RAND_MAX - 0.5))*dy;
                    if (NGRIDX%2 == 0) circlex[n] += 0.5*dy;
                    y = ((double)j + 0.5 + 0.5*((double)rand()/RAND_MAX - 0.5))*dy;
                    if (top) circley[n] = ymean + y;
                    else circley[n] = ymean - y;
                    circlerad[n] = MU*sqrt(1.0 + 0.8*((double)rand()/RAND_MAX - 0.5));
                    circleactive[n] = 1;
                    circletop[n] = top;
                    printf("n = %i, x = %.3lg\n", n, circlex[n]);
                }
            ncircles += NGRIDX*NGRIDY/2;
            break;
        }
        case (C_RAND_PERCOL):
        {
            dy = (YMAX - YMIN)/((double)NGRIDY);
            for (i = 0; i < NGRIDX; i++)
                for (j = 0; j < NGRIDY/2; j++)
                {
                    n = ncircles + NGRIDY*i/2 + j;
                    circlex[n] = ((double)(i-NGRIDX/2) + 0.5)*dy;
                    y = YMIN + ((double)j + 0.5)*dy;
                    if ( ((top)&&(y > 0.0))||((!top)&&(y <= 0.0)) )
                        circley[n] = y;
                    circlerad[n] = MU;
                    p = (double)rand()/RAND_MAX;
                    if (p < P_PERCOL) circleactive[n] = 1;
                    else circleactive[n] = 0;
                    circletop[n] = top;
                }
            ncircles += NGRIDX*NGRIDY/2;
            break;
        }
        case (C_RAND_POISSON):
        {
            for (i = 0; i < NPOISSON/2; i++)
            {
                n = ncircles + i;
                circlex[n] = LAMBDA*(2.0*(double)rand()/RAND_MAX - 1.0);
                if (top) y = ymean + (YMAX-ymean)*(double)rand()/RAND_MAX;
                else y = ymean + (YMIN-ymean)*(double)rand()/RAND_MAX;
                if ( ((top)&&(y > 0.0))||((!top)&&(y <= 0.0)) )
                    circley[n] = y;
                circlerad[n] = MU;
                circleactive[n] = 1;
                circletop[n] = top;
                printf("n = %i, x = %.3lg\n", n, circlex[n]);
            }
            ncircles += NPOISSON/2;
            break;
        }
        case (C_CLOAK):
        {
            for (i = 0; i < 40; i++)
                for (j = 0; j < 5; j++)
                {
                    n = ncircles + 5*i + j;
                    phi = (double)i*DPI/40.0;
                    r = LAMBDA*0.5*(1.0 + (double)j/5.0);
                    circlex[n] = r*cos(phi);
                    y = r*sin(phi);
                    if ( ((top)&&(y > 0.0))||((!top)&&(y <= 0.0)) )
                        circley[n] = y;
                    circlerad[n] = MU;
                    circleactive[n] = 1;
                    circletop[n] = top;
                }
            ncircles += 200;
            break;
        }
        case (C_CLOAK_A):   /* optimized model A1 by C. Jo et al */
        {
            ra[0] = 0.0731;     sa[0] = 1.115;
            ra[1] = 0.0768;     sa[1] = 1.292;
            ra[2] = 0.0652;     sa[2] = 1.464;
            ra[3] = 0.056;      sa[3] = 1.633;
            ra[4] = 0.0375;     sa[4] = 1.794;
            for (i = 0; i < 21; i++)
                for (j = 0; j < 5; j++)
                {
                    n = ncircles + 5*i + j;
                    phi = (double)i*DPI/40.0;
                    r = LAMBDA*sa[j];
                    circlex[n] = r*cos(phi);
                    circley[n] = r*sin(phi);
                    
                    if (top) y = r*sin(phi);
                    else y = -r*sin(phi);
                        
                    circley[n] = y;
                    
                    circlerad[n] = LAMBDA*ra[j];
                    circleactive[n] = 1;
                    circletop[n] = top;
                }
            ncircles += 105;
            
            /* add circle in the center */
            circlex[ncircles] = 0.0;
            circley[ncircles] = 0.0;
            circlerad[ncircles] = MU;
            circleactive[ncircles] = 2;
            circletop[ncircles] = top; 
            ncircles += 1;
            break;
        }
        case (C_POISSON_DISC):
        {
            printf("Generating Poisson disc sample\n");
            /* generate first circle */
            n = ncircles;
            circlex[n] = LAMBDA*(2.0*(double)rand()/RAND_MAX - 1.0);
            if (top) y = ymean + (YMAX-ymean)*(double)rand()/RAND_MAX;
            else y = ymean + (YMIN-ymean)*(double)rand()/RAND_MAX;
            circley[n] = y;
            circlerad[n] = MU;
            circleactive[n] = 1;
            circletop[n] = top;
            active_poisson[n] = 1;
            n_p_active = 1;
            ncirc0 = 1;
            
            while ((n_p_active > 0)&&(ncircles + ncirc0 < NMAXCIRCLES))
            {
                /* randomly select an active circle */
                i = rand()%(ncirc0);
                n = ncircles + i;
                while (!active_poisson[ncircles + i]) i = rand()%(ncirc0);                 
//                 printf("Starting from circle %i at (%.3f,%.3f)\n", i, circlex[i], circley[i]);
                /* generate new candidates */
                naccepted = 0;
                for (j=0; j<ncandidates; j++)
                {
                    r = dpoisson*(2.0*(double)rand()/RAND_MAX + 1.0);
                    phi = DPI*(double)rand()/RAND_MAX;
                    x = circlex[n] + r*cos(phi);
                    y = circley[n] + r*sin(phi);
//                        printf("Testing new circle at (%.3f,%.3f)\t", x, y);
                    far = 1;
                    for (k=ncircles; k<ncircles + ncirc0; k++) if ((k!=n))
                    {
                        /* new circle is far away from circle k */
                        far = far*((x - circlex[k])*(x - circlex[k]) + (y - circley[k])*(y - circley[k]) >= dpoisson*dpoisson);
                        /* new circle is in domain */
                        if (top) far = far*(vabs(x) < LAMBDA)*(y < YMAX)*(y > 0.0);
                        else far = far*(vabs(x) < LAMBDA)*(y > YMIN)*(y < 0.0);
                    }
                    if (far)    /* accept new circle */
                    {
                        printf("New circle at (%.3f,%.3f) accepted\n", x, y);
                        nnew = ncircles + ncirc0;
                        circlex[nnew] = x;
                        circley[nnew] = y;
                        circlerad[nnew] = MU;
                        circleactive[nnew] = 1;
                        active_poisson[nnew] = 1;
                        circleactive[nnew] = 1;
                        circletop[nnew] = top;
                        ncirc0++;
                        n_p_active++;
                        naccepted++;
                    }
//                     else printf("Rejected\n");
                }
                if (naccepted == 0)    /* inactivate circle i */ 
                {
//                     printf("No candidates work, inactivate circle %i\n", ncircles + i);
                    active_poisson[ncircles + i] = 0;
                    n_p_active--;
                }
                printf("%i active circles\n", n_p_active);
//                 sleep(1);
            }
            
            printf("Already existing: %i circles\n", ncircles);
            ncircles += ncirc0;
            printf("Generated %i circles\n", ncirc0);
            printf("Total: %i circles\n", ncircles);
            break;
        }
        case (C_GOLDEN_MEAN):
        {
            gamma = (sqrt(5.0) - 1.0)*0.5;    /* golden mean */
            height = YMAX - ymean;
            dx = 2.0*LAMBDA/150.0;
            if (top) y = ymean + 0.5*height;
            else y = ymean - 0.5*height; 
            for (n = 0; n < 150; n++)
            {
                circlex[ncircles + n] = -LAMBDA + n*dx;
                circley[ncircles + n] = y;
                
                if (top)
                {
                    y += height*gamma; 
                    if (y > YMAX) y -= height;
                }
                else 
                {
                    y -= height*gamma; 
                    if (y < YMIN) y += height;
                   
                }
                
                circlerad[ncircles + n] = MU;
                circleactive[ncircles + n] = 1;
                circletop[ncircles] = top; 
            }
            
            /* test for circles that overlap top or bottom boundary */
            ncirc0 = ncircles;
            ncircles += 150;
            if (top) ytop = YMAX;
            else ytop = ymean;
            
            if (top) ybottom = ymean;
            else ybottom = YMIN;
            
            for (n=0; n < 150; n++)
            {
                if (circley[ncirc0 + n] + circlerad[ncirc0 + n] > ytop)
                {
                    circlex[ncircles] = circlex[ncirc0 + n];
                    circley[ncircles] = circley[ncirc0 + n] - height;
                    circlerad[ncircles] = MU;
                    circleactive[ncircles] = 1;
                    ncircles ++;
                }
                else if (circley[ncirc0 + n] - circlerad[ncirc0 + n] < ybottom)
                {
                    circlex[ncircles] = circlex[ncirc0 + n];
                    circley[ncircles] = circley[ncirc0 + n] + height;
                    circlerad[ncircles] = MU;
                    circleactive[ncircles] = 1;
                    ncircles ++;
                }
            }
            
            break;
        }
        case (C_GOLDEN_SPIRAL):
        {
            circlex[ncircles] = 0.0;
            circley[ncircles] = 0.0;
            circlerad[ncircles] = MU;
            circleactive[ncircles] = 1;
            circletop[ncircles] = top; 
            
            gamma = (sqrt(5.0) - 1.0)*PI;    /* golden mean times 2Pi */
            phi = 0.0;
            r0 = 2.0*MU;
            r = r0 + MU;
            
            for (i=0; i<1000; i++) 
            {
                x = r*cos(phi);
                y = r*sin(phi);
                
                phi += gamma;
                r += MU*r0/r;
                
                if ((vabs(x) < LAMBDA)&&(vabs(y) < YMAX + MU))
                {
                    circlex[ncircles] = x;
                    circley[ncircles] = y;
                    circlerad[ncircles] = MU;
                    if (((top)&&(circley[ncircles] < YMAX + MU)&&(circley[ncircles] > ymean - MU))
                        ||((!top)&&(circley[ncircles] < ymean + MU)&&(circley[ncircles] > YMIN - MU)))
                    {
                        circleactive[ncircles] = 1;
                        circletop[ncircles] = top; 
                        ncircles++;
                    }
                }
            }
        break;
        }
        case (C_ONE):
        {
            circlex[ncircles] = 0.0;
            circley[ncircles] = 0.0;
            if (top) circlerad[ncircles] = MU;
            else circlerad[ncircles] = MUB;
            circleactive[ncircles] = 1;
            circletop[ncircles] = top; 
            ncircles += 1;
            break;
        }
        case (C_TWO):
        {
            circlex[ncircles] = 0.0;
            circley[ncircles] = 0.0;
            if (top) circlerad[ncircles] = MU;
            else circlerad[ncircles] = MUB;
            circleactive[ncircles] = 2;
            circletop[ncircles] = top; 
            ncircles += 1;

            circlex[ncircles] = 0.0;
            circley[ncircles] = 0.0;
            if (top) circlerad[ncircles] = 2.0*MU;
            else circlerad[ncircles] = 2.0*MUB;
            circleactive[ncircles] = 1;
            circletop[ncircles] = top; 
            ncircles += 1; 
            break;
        }
        case (C_NOTHING):
        {
            ncircles += 0;
            break;
        }
        default: 
        {
            printf("Function init_circle_config not defined for this pattern \n");
        }
    }
}


void init_circle_config_comp()
/* initialise the arrays circlex, circley, circlerad and circleactive */
/* for billiard shape D_CIRCLES */
{
    ncircles = 0;
    init_circle_config_half(CIRCLE_PATTERN, 1);
    init_circle_config_half(CIRCLE_PATTERN_B, 0);
}

void init_circle_config_energy()
/* initialise the arrays circlex, circley, circlerad and circleactive */
/* for billiard shape D_CIRCLES */
{
    ncircles = 0;
    init_circle_config_half(CIRCLE_PATTERN, 0);
}

int xy_in_billiard_half(double x, double y, int domain, int pattern, int top)
/* returns 1 if (x,y) represents a point in the billiard */
{
    double l2, r2, r2mu, omega, c, angle, z, x1, y1, x2, y2, u, v, u1, v1, dx, dy;
    int i, j, k, k1, k2, condition, type;

    switch (domain) {
        case D_MENGER:       
        {
            x1 = 0.5*(x+1.0);
            y1 = 0.5*(y+1.0);
            for (k=0; k<MDEPTH; k++)
            {
                x1 = x1*(double)MRATIO;
                y1 = y1*(double)MRATIO;
                if ((top)&&(y > 0.0)&&(vabs(x)<1.0)&&(vabs(y)<1.0)&&(((int)x1 % MRATIO)==MRATIO/2)&&(((int)y1 % MRATIO)==MRATIO/2))                 return(0);
                else if ((!top)&&(y < 0.0)&&(vabs(x)<1.0)&&(vabs(y)<1.0)&&(((int)x1 % MRATIO)==MRATIO/2)&&(((int)y1 % MRATIO)==MRATIO/2)) return(0); 
            }
            return(1);
        }
        case D_CIRCLES:
        {
            for (i = 0; i < ncircles; i++)
                if (circleactive[i] != 0) 
                {
                    /* choose specific type according to value of circleactive[i] */
                    if (circleactive[i] == 1) type = 0;
                    else type = circleactive[i];
                    
                    x1 = circlex[i];
                    y1 = circley[i];
                    r2 = circlerad[i]*circlerad[i];
                    if ((top)&&(circletop[i])&&(y > 0.0)&&((x-x1)*(x-x1) + (y-y1)*(y-y1) < r2)) return(type); 
                    else if ((!top)&&(!circletop[i])&&(y < 0.0)&&((x-x1)*(x-x1) + (y-y1)*(y-y1) < r2)) return(type); 
                }
            return(1);
        }
        default:
        {
            printf("Function ij_in_billiard not defined for this billiard \n");
            return(0);
        }
    }
}


int xy_in_billiard_comp(double x, double y)
/* returns 1 if (x,y) represents a point in the billiard */
{
    if (y > 0) return(xy_in_billiard_half(x, y, B_DOMAIN, CIRCLE_PATTERN, 1));
    else return(xy_in_billiard_half(x, y, B_DOMAIN_B, CIRCLE_PATTERN_B, 0));
}


int ij_in_billiard_comp(int i, int j)
/* returns 1 if (i,j) represents a point in the billiard */
{
    double xy[2];

    ij_to_xy(i, j, xy);

    return(xy_in_billiard_comp(xy[0], xy[1]));
}


void draw_billiard_half(int domain, int pattern, int top)      /* draws the billiard boundary */
/* two domain version implemented for D_CIRCLES */
{
    double x0, x, y, x1, y1, dx, dy, phi, r = 0.01, pos[2], pos1[2], alpha, dphi, omega, z, l, signtop;
    int i, j, k, k1, k2, mr2;

    if (BLACK) glColor3f(1.0, 1.0, 1.0);
    else glColor3f(0.0, 0.0, 0.0);
    glLineWidth(5);
    
    if (top) signtop = 1.0;
    else signtop = -1.0;

    glEnable(GL_LINE_SMOOTH);

    switch (domain) {
        case (D_MENGER):
        {
            glLineWidth(3);
//             draw_rectangle(XMIN, -1.0, XMAX, 1.0);
            
            /* level 1 */
            if (MDEPTH > 0)
            {
                glLineWidth(2);
                x = 1.0/((double)MRATIO);
                draw_rectangle(-x, 0.0, x, signtop*x);
            }
            
            /* level 2 */
            if (MDEPTH > 1)
            {
                glLineWidth(1);
                mr2 = MRATIO*MRATIO;
                l = 2.0/((double)mr2);
                
                for (i=0; i<MRATIO; i++)
                    for (j=MRATIO/2; j<MRATIO; j++)
                        if ((i!=MRATIO/2)||(j!=MRATIO/2))
                        {
                            x = -1.0 - 0.5*l + 2.0*((double)i + 0.5)/((double)MRATIO);
                            y = -1.0 - 0.5*l + 2.0*((double)j + 0.5)/((double)MRATIO);
                            y1 = y+l;
                            if (y < 0.0) y = 0.0;
                            if (y1 < 0.0) y1 = 0.0;
                            draw_rectangle(x, signtop*y, x+l, signtop*y1);
                        }
            }
            
            /* level 3 */
            if (MDEPTH > 2)
            {
                glLineWidth(1);
                l = 2.0/((double)(mr2*MRATIO));
                
                for (i=0; i<mr2; i++)
                    for (j=mr2/2; j<mr2; j++)
                        if ( (((i%MRATIO!=MRATIO/2))||(j%MRATIO!=MRATIO/2)) && (((i/MRATIO!=MRATIO/2))||(j/MRATIO!=MRATIO/2)) )
                        {
                            x = -1.0 - 0.5*l + 2.0*((double)i + 0.5)/((double)mr2);
                            y = -1.0 - 0.5*l + 2.0*((double)j + 0.5)/((double)mr2);
                            y1 = y+l;
                            if (y < 0.0) y = 0.0;
                            if (y1 < 0.0) y1 = 0.0;
                            draw_rectangle(x, signtop*y, x+l, signtop*y1);
                        }
            }
            
            break;
        }        
        case (D_CIRCLES):
        {
            glLineWidth(2);
            for (i = 0; i < ncircles; i++) 
                if ((circleactive[i])&&(circletop[i] == top)) 
                {
                    glBegin(GL_LINE_STRIP);
                    for (k=0; k<=NSEG; k++)
                    {
                        phi = (double)k*DPI/(double)NSEG;
                        x = circlex[i] + circlerad[i]*cos(phi);
                        y = circley[i] + circlerad[i]*sin(phi);
                        if ((top)&&(circletop[i])&&(y < 0.0)) y = 0.0;
                        if ((!top)&&(!circletop[i])&&(y > 0.0)) y = 0.0;
                        xy_to_pos(x, y, pos);
                        glVertex2d(pos[0], pos[1]);
                    }
                    glEnd ();
                }
            break;
        }
        case (D_MANDELBROT):
        {
            /* Do nothing */
            break;
        }
        case (D_MANDELBROT_CIRCLE):
        {
            /* Do nothing */
            break;
        }
        case (D_JULIA):
        {
            /* Do nothing */
            break;
        }
       default:
        {
            printf("Function draw_billiard not defined for this billiard \n");
        }
    }
}


void draw_billiard_comp()      /* draws the billiard boundary */
{
    draw_billiard_half(B_DOMAIN, CIRCLE_PATTERN, 1);
    draw_billiard_half(B_DOMAIN_B, CIRCLE_PATTERN_B, 0);
}


void int_planar_wave_comp(double x, double y, double *phi[NX], double *psi[NX], short int * xy_in[NX])
/* initialise field with drop at (x,y) - phi is wave height, psi is phi at time t-1 */
/* beta version, works for vertical planar wave only so far */
{
    int i, j;
    double xy[2], dist2;

    for (i=0; i<NX; i++)
        for (j=0; j<NY; j++)
        {
            ij_to_xy(i, j, xy);
            dist2 = (xy[0]-x)*(xy[0]-x);
	    xy_in[i][j] = xy_in_billiard_comp(xy[0],xy[1]);
            
	    if ((xy_in[i][j])||(TWOSPEEDS)) phi[i][j] = 0.01*exp(-dist2/0.0005)*cos(-sqrt(dist2)/0.01);
            else phi[i][j] = 0.0;
            psi[i][j] = 0.0;
        }
}


void init_wave_flat_comp( double *phi[NX], double *psi[NX], short int * xy_in[NX])
/* initialise flat field - phi is wave height, psi is phi at time t-1 */
{
    int i, j;
    double xy[2], dist2;

    for (i=0; i<NX; i++)
        for (j=0; j<NY; j++)
        {
            ij_to_xy(i, j, xy);
	    xy_in[i][j] = xy_in_billiard_comp(xy[0],xy[1]);
	    phi[i][j] = 0.0;
            psi[i][j] = 0.0;
        }
}


void draw_wave_comp(double *phi[NX], double *psi[NX], short int *xy_in[NX], double scale, int time)
/* draw the field */
{
    int i, j, iplus, iminus, jplus, jminus;
    double rgb[3], xy[2], x1, y1, x2, y2, velocity, energy, gradientx2, gradienty2, pos[2];
    static double dtinverse = ((double)NX)/(COURANT*(XMAX-XMIN)), dx = (XMAX-XMIN)/((double)NX);

    glBegin(GL_QUADS);
    
//     printf("dtinverse = %.5lg\n", dtinverse);

    for (i=0; i<NX; i++)
        for (j=0; j<NY; j++)
        {
            if (((TWOSPEEDS)&&(xy_in[i][j] != 2))||(xy_in[i][j] == 1))
            {
                if (PLOT == P_AMPLITUDE)
                    color_scheme(COLOR_SCHEME, phi[i][j], scale, time, rgb);
                else if (PLOT == P_ENERGY)
                    color_scheme(COLOR_SCHEME, compute_energy(phi, psi, xy_in, i, j), scale, time, rgb);
                else if (PLOT == P_MIXED)
                {
                    if (j > NY/2) color_scheme(COLOR_SCHEME, phi[i][j], scale, time, rgb);
                    else color_scheme(COLOR_SCHEME, compute_energy(phi, psi, xy_in, i, j), scale, time, rgb);
                }
                glColor3f(rgb[0], rgb[1], rgb[2]);

                glVertex2i(i, j);
                glVertex2i(i+1, j);
                glVertex2i(i+1, j+1);
                glVertex2i(i, j+1);
            }
        }

    glEnd ();
    
    /* draw horizontal mid line */
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    xy_to_pos(XMIN, 0.5*(YMIN+YMAX), pos);
    glVertex2d(pos[0], pos[1]);
    xy_to_pos(XMAX, 0.5*(YMIN+YMAX), pos);
    glVertex2d(pos[0], pos[1]);
    glEnd();
}

void compute_energy_tblr(double *phi[NX], double *psi[NX], short int *xy_in[NX], double energies[6])
/* compute total energy in top/bottom left/right boxes */ 
{
    int i, j, ij[2];
    double energy = 0.0, rescale, pos, xleft = XMAX, xright = XMIN, emax = 1.2;
    double energy_ij[NX][NY];
    static short int first = 1;
    static int ileft, iright, jmid = NY/2; 
    static double sqremax;
    
    
    if (first) /* compute box limits */
    {
        /* find leftmost and rightmost circle */
        for (i=0; i<ncircles; i++) 
            if ((circleactive[i])&&(circlex[i] - circlerad[i] < xleft)) xleft = circlex[i] - circlerad[i]; 
        for (i=0; i<ncircles; i++) 
            if ((circleactive[i])&&(circlex[i] + circlerad[i] > xright)) xright = circlex[i] + circlerad[i]; 
        
        xy_to_ij(xleft, 0.0, ij);
        ileft = ij[0];
        xy_to_ij(xright, 0.0, ij);
        iright = ij[0];
        first = 0;
        
        printf("xleft = %.3lg, xright = %.3lg", xleft, xright);
    }
    
    for (i=0; i<NX; i++)
        for (j=0; j<NY; j++)
            energy_ij[i][j] = compute_energy(phi, psi, xy_in, i, j);
    
    /* prevent local energy from growing too large */
    if (FLOOR)
    {
        for (i=10; i<NX; i++)
            for (j=0; j<NY; j++)
                if ((xy_in[i][j])&&(energy_ij[i][j] > emax))
                {
                    rescale = sqrt(emax/energy_ij[i][j]);
                    if (j%100 == 0) printf("Rescaling at (%i,%i) by %.5lg\n", i, j, rescale);
                    phi[i][j] = phi[i][j]*rescale;
                    psi[i][j] = psi[i][j]*rescale;
                }
                else if (energy_ij[i][j] > 0.1*emax)
                {
                    rescale = sqrt(0.1*emax/energy_ij[i][j]);
                    if (j%10 == 0) printf("Rescaling at (%i,%i) by %.5lg\n", i, j, rescale);
                    phi[i][j] = phi[i][j]*rescale;
                    psi[i][j] = psi[i][j]*rescale;
                }
    }
    
    /* top left box */
    for (i=0; i<ileft; i++)
        for (j=jmid; j<NY; j++)
            energy += energy_ij[i][j];
    energies[0] = energy;
    
    /* top middle box */
    energy = 0.0;
    for (i=ileft; i<iright; i++)
        for (j=jmid; j<NY; j++)
            energy += energy_ij[i][j];
    energies[1] = energy;
    
    /* top right box */
    energy = 0.0;
    for (i=iright; i<NX; i++)
        for (j=jmid; j<NY; j++)
            energy += energy_ij[i][j];
    energies[2] = energy;
    
    /* bottom left box */
    energy = 0.0;
    for (i=0; i<ileft; i++)
        for (j=0; j<jmid; j++)
            energy += energy_ij[i][j];
    energies[3] = energy;
    
    /* bottom middle box */
    energy = 0.0;
    for (i=ileft; i<iright; i++)
        for (j=0; j<jmid; j++)
            energy += energy_ij[i][j];
    energies[4] = energy;
    
    /* bottom right box */
    energy = 0.0;
    for (i=iright; i<NX; i++)
        for (j=0; j<jmid; j++)
            energy += energy_ij[i][j];
    energies[5] = energy;
    
//     printf("Energies: %.5lg, %.5lg, %.5lg\n %.5lg, %.5lg, %.5lg\n", energies[0], energies[1], energies[2], energies[3], energies[4], energies[5]);
    
}

void print_energies(double energies[6], double top_energy, double bottom_energy)
{
    char message[50];
    double ytop, ybot, pos[2], centerx = -0.075;
    
    ytop = YMAX - 0.1;
//     ybot = -0.1;
    ybot = YMIN + 0.05;
    
    erase_area(XMIN + 0.175, ytop + 0.025, 0.1, 0.05);

    glColor3f(1.0, 1.0, 1.0);
    sprintf(message, "%.3f", energies[0]/top_energy);
    xy_to_pos(XMIN + 0.1, ytop, pos);
    write_text(pos[0], pos[1], message);
    
    erase_area(centerx + 0.075, ytop + 0.025, 0.1, 0.05);

    glColor3f(1.0, 1.0, 1.0);
    sprintf(message, "%.3f", energies[1]/top_energy);
    xy_to_pos(centerx, ytop, pos);
    write_text(pos[0], pos[1], message);
    
    glColor3f(1.0, 1.0, 1.0);
    sprintf(message, "%.5f", energies[2]/top_energy);
    xy_to_pos(XMAX - 0.28, ytop, pos);
    write_text(pos[0], pos[1], message);
    
    erase_area(XMIN + 0.175, ybot + 0.025, 0.1, 0.05);

    glColor3f(1.0, 1.0, 1.0);
    sprintf(message, "%.3f", energies[3]/bottom_energy);
    xy_to_pos(XMIN + 0.1, ybot, pos);
    write_text(pos[0], pos[1], message);

    erase_area(centerx + 0.075, ybot + 0.025, 0.1, 0.05);

    glColor3f(1.0, 1.0, 1.0);
    sprintf(message, "%.3f", energies[4]/bottom_energy);
    xy_to_pos(centerx, ybot, pos);
    write_text(pos[0], pos[1], message);

    glColor3f(1.0, 1.0, 1.0);
    sprintf(message, "%.5f", energies[5]/bottom_energy);
    xy_to_pos(XMAX - 0.28, ybot, pos);
    write_text(pos[0], pos[1], message);
    
}

