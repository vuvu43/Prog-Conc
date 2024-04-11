import matplotlib.pyplot as plt
import pandas as pd

data = {  ### Dados da execução para matrizes 2000 x 2000
    "Inicialização": [0.002792, 0.028295, 0.065650, 0.096776, 0.052337],
    "Multiplicação": [417.980282, 143.195680, 94.356243, 73.569056, 67.495091],
    "Finalização": [0.000779, 0.000908, 0.000789, 0.000708, 0.000827]
}

df = pd.DataFrame(data)

# Definindo os pontos no eixo x
threads = [0, 1, 2, 4, 8]

# Calculando aceleração
df_normalized = df.iloc[0] / df

# Plotando a aceleração
for column in df_normalized.columns:
    plt.figure(figsize=(8, 5))
    plt.plot(threads, df_normalized[column], marker='o', color='darkgreen')
    plt.xlabel('Quantidade de Threads')
    plt.ylabel('Aceleração')
    plt.title(f'Aceleração x Quantidade de Threads - {column}')
    plt.grid(True)
    plt.show()

# Achando a eficiência
df_modified = df_normalized / 2

# Plotando a eficiência
for column in df_modified.columns:
    plt.figure(figsize=(8, 5))
    plt.plot(threads[1:], df_modified[column][1:], marker='o', color='darkgreen') #Eficiência sequencial não é relevante
    plt.xlabel('Quantidade de Threads')
    plt.ylabel('Eficiência')
    plt.title(f'Eficiência x Quantidade de Threads - {column}')
    plt.grid(True)
    plt.show()
