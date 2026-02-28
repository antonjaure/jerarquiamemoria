#!/bin/bash

# Comprobar si se ha pasado un archivo como argumento
if [ "$#" -ne 1 ]; then
    echo "Uso: $0 archivo_datos.txt"
    exit 1
fi

# LC_NUMERIC=C asegura que el punto decimal se trate correctamente siempre
LC_NUMERIC=C awk '
BEGIN {
    # Definimos alta precisión para cálculos internos y salida
    CONVFMT = "%.15g"
    printf "%-8s %-10s %-20s\n", "D", "L", "Media Geo (Top 3)"
    print "----------------------------------------------------"
}
{
    # Filtramos líneas: deben tener 3 columnas numéricas
    # Columna 1 (D), Columna 2 (L), Columna 3 (Ciclos)
    if ($1 ~ /^[0-9]+$/ && $2 ~ /^[0-9]+$/ && $3 ~ /^[0-9.]+$/) {
        key = $1 "\t" $2
        # Guardamos el valor forzándolo a numérico con +0
        datos[key] = datos[key] ($3 + 0) " "
    }
}
END {
    for (k in datos) {
        n = split(datos[k], valores, " ")
        
        # Ordenamos los ciclos de menor a mayor (Bubble Sort)
        for (i = 1; i <= n; i++) {
            for (j = i + 1; j <= n; j++) {
                if (valores[i] > valores[j]) {
                    tmp = valores[i]
                    valores[i] = valores[j]
                    valores[j] = tmp
                }
            }
        }

        # Calculamos media geométrica de los 3 mejores (menores)
        limite = (n < 3) ? n : 3
        if (limite > 0) {
            suma_logs = 0
            for (i = 1; i <= limite; i++) {
                suma_logs += log(valores[i])
            }
            # Media Geométrica = exp( suma(log(x)) / n )
            media_geo = exp(suma_logs / limite)
            
            # Imprimimos con 6 decimales de precisión
            printf "%-19s %-20.6f\n", k, media_geo
        }
    }
}' "$1" | sort -n -k1,1 -k2,2