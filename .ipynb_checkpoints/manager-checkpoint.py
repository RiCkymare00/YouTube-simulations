import re

def leggi_define(file_path):
    defines = {}
    with open(file_path, 'r') as file:
        for line in file:
            match = re.match(r'#define\s+(\w+)\s+(.+)', line)
            if match:
                nome_var, valore = match.groups()
                defines[nome_var] = valore.strip()
    return defines

def aggiorna_define(file_rde, defines_impostazioni):
    nuove_righe = []
    modifiche = []

    with open(file_rde, 'r') as file:
        for line in file:
            match = re.match(r'#define\s+(\w+)\s+(.+)', line)
            if match:
                nome_var, valore_corrente = match.groups()
                # Se la variabile è presente in impostazioni.c, sostituisci il valore
                if nome_var in defines_impostazioni:
                    valore_nuovo = defines_impostazioni[nome_var]
                    if valore_corrente != valore_nuovo:
                        line = f"#define {nome_var} {valore_nuovo}\n"
                        modifiche.append(f"Sostituito {nome_var}: {valore_corrente} -> {valore_nuovo}")
            nuove_righe.append(line)
    
    # Sovrascrivi rde.c con le nuove righe aggiornate
    with open(file_rde, 'w') as file:
        file.writelines(nuove_righe)
    
    return modifiche

# Percorsi dei file
file_impostazioni = 'impostazioni.c'
file_rde = 'rde_test.c'

# Legge i define da impostazioni.c
defines_impostazioni = leggi_define(file_impostazioni)

# Aggiorna i define in rde.c con quelli di impostazioni.c e stampa le modifiche
modifiche = aggiorna_define(file_rde, defines_impostazioni)
print("Modifiche apportate:")
for modifica in modifiche:
    print(modifica)


