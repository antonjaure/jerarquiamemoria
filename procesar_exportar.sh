#!/bin/bash

# Comprobar si se ha pasado un archivo como argumento
if [ "$#" -ne 1 ]; then
    echo "Uso: $0 archivo_datos.txt"
    exit 1
fi

# Nombre del archivo de salida
OUTPUT_CSV="resultados_media_geo3.csv"

# Usamos LC_NUMERIC=C para asegurar que el punto (.) se trate como decimal
LC_NUMERIC=C awk -v csv_file="$OUTPUT_CSV" '
BEGIN {
    # Precisión interna de 15 dígitos
    CONVFMT = "%.15g"
    
    # Encabezados para la consola
    printf "%-8s %-10s %-20s\n", "D", "L", "Media Geo (Top 3)"
    print "----------------------------------------------------"
    
    # Encabezado para el CSV (usamos ; como separador)
    print "D;L;Media_Geometrica_Ciclos" > csv_file
}
{
    # Filtro de líneas con 3 columnas numéricas (D, L, Ciclos)
    if ($1 ~ /^[0-9]+$/ && $2 ~ /^[0-9]+$/ && $3 ~ /^[0-9.]+$/) {
        key = $1 "\t" $2
        datos[key] = datos[key] ($3 + 0) " "
    }
}
END {
    for (k in datos) {
        n = split(datos[k], valores, " ")
        
        # Bubble Sort para obtener los menores (mejores)
        for (i = 1; i <= n; i++) {
            for (j = i + 1; j <= n; j++) {
                if (valores[i] > valores[j]) {
                    tmp = valores[i]
                    valores[i] = valores[j]
                    valores[j] = tmp
                }
            }
        }

        # Media geométrica de los 3 mejores usando logaritmos
        limite = (n < 3) ? n : 3
        if (limite > 0) {
            suma_logs = 0
            for (i = 1; i <= limite; i++) {
                suma_logs += log(valores[i])
            }
            media_geo = exp(suma_logs / limite)
            
            # Formatear la clave para el CSV (cambiar tab por ;)
            split(k, parts, "\t")
            
            # Guardar en CSV con 6 decimales
            printf "%s;%s;%.6f\n", parts[1], parts[2], media_geo >> csv_file
            
            # Mostrar en consola
            printf "%-19s %-20.6f\n", k, media_geo
        }
    }
}' "$1" | sort -n -k1,1 -k2,2

echo "----------------------------------------------------"
echo "¡Listo! Los resultados se han guardado en: $OUTPUT_CSV"
