import os, sys
project_directory = os.getcwd()
sys.path.append(project_directory)

import numpy as np
import pandas as pd
from sklearn.metrics import accuracy_score
import random

import matplotlib.pyplot as plt

linestyle_tuple = [
    ('solid', 'solid'),      # Same as (0, ()) or '-'
    ('dotted', 'dotted'),    # Same as (0, (1, 1)) or ':'
    ('dashed', 'dashed'),    # Same as '--'
    ('dashdot', 'dashdot'),
    ('densely dotted',        (0, (1, 1))),
    ('loosely dotted',        (0, (1, 10))),
    ('dotted',                (0, (1, 1))),
    ('long dash with offset', (5, (10, 3))),
    ('loosely dashed',        (0, (5, 10))),
    ('dashed',                (0, (5, 5))),
    ('densely dashed',        (0, (5, 1))),

    ('loosely dashdotted',    (0, (3, 10, 1, 10))),
    ('dashdotted',            (0, (3, 5, 1, 5))),
    ('densely dashdotted',    (0, (3, 1, 1, 1))),

    ('dashdotdotted',         (0, (3, 5, 1, 5, 1, 5))),
    ('loosely dashdotdotted', (0, (3, 10, 1, 10, 1, 10))),
    ('densely dashdotdotted', (0, (3, 1, 1, 1, 1, 1)))
]

def pp(resume, algoritm_name):
    vet = []

    for algorithm in resume.columns:
        resume_ = resume[algorithm].values
        resume_ = sorted(resume_)

        linha = [1]

        for i in range(0, len(resume_)):
            linha.append(resume_[i])
            linha.append(resume_[i])

        linha = np.array(linha)
        linha = linha.T

        vet.append(linha)

    vet = np.array(vet)
    vet = vet.T

    maior = np.max(vet)
    maior = [[maior for _ in range(len(resume.columns))]]
    vet = np.concatenate([vet, np.array(maior)])

    vet = pd.DataFrame(vet)
    vet.columns = resume.columns
    names = vet.columns

    h = 1. / len(resume)
    vet['h'] = [h*(i//2) for i in range(len(vet))]

    areas = {}
    for algorithm in names:
        area = 0
        for i in range(0, len(vet), 2):
            area = area + (vet[algorithm][i + 1] - vet[algorithm][i]) * vet['h'][i]
        areas[algorithm] = area

    # Normaliza as áreas pelo maior valor encontrado
    max_area = max(areas.values())
    for i in range(len(names)):
        areas[names[i]] = areas[names[i]] / max_area

    # Ordena as áreas em ordem decrescente e reorganiza os nomes
    areas = dict(sorted(areas.items(), key=lambda item: item[1], reverse=1))
    # areas = dict(sorted(areas.items(), key=lambda item: item[1], reverse=True))
    names = list(areas.keys())[:]

    fig, ax = plt.subplots(figsize=(10, 10))

    # Gráfico de linhas à esquerda (ax1) representando os perfis de desempenho
    for i in range(len(names)):
        ax.plot(vet[names[i]], vet['h'], linestyle = linestyle_tuple[i % len(linestyle_tuple)][1], label=names[i])

    # Configurações do eixo ax1 (gráfico de linhas)
    ax.set_title("Performance Profiles")                                      
    ax.set_xlabel(fr'$\tau$')
    ax.set_ylabel(fr'$\rho(\tau)$')
    ax.set_xscale('log')
    ax.legend()

    # Ajusta automaticamente a disposição dos subplots para evitar sobreposição
    plt.tight_layout()
    # Exibe o gráfico
    plt.savefig(f"{algoritm_name}.svg")
    plt.show()

    return areas

def pp_auc(resume, algoritm_name):
    vet = []

    for algorithm in resume.columns:
        resume_ = resume[algorithm].values
        resume_ = sorted(resume_)

        linha = [1]

        for i in range(0, len(resume_)):
            linha.append(resume_[i])
            linha.append(resume_[i])

        linha = np.array(linha)
        linha = linha.T

        vet.append(linha)

    vet = np.array(vet)
    vet = vet.T

    maior = np.max(vet)
    maior = [[maior for _ in range(len(resume.columns))]]
    vet = np.concatenate([vet, np.array(maior)])

    vet = pd.DataFrame(vet)
    vet.columns = resume.columns
    names = vet.columns

    h = 1. / len(resume)
    vet['h'] = [h*(i//2) for i in range(len(vet))]

    areas = {}
    for algorithm in names:
        area = 0
        for i in range(0, len(vet), 2):
            area = area + (vet[algorithm][i + 1] - vet[algorithm][i]) * vet['h'][i]
        areas[algorithm] = area

    # Normaliza as áreas pelo maior valor encontrado
    max_area = max(areas.values())
    for i in range(len(names)):
        areas[names[i]] = areas[names[i]] / max_area

    # Ordena as áreas em ordem decrescente e reorganiza os nomes
    areas = dict(sorted(areas.items(), key=lambda item: item[1], reverse=1))
    # areas = dict(sorted(areas.items(), key=lambda item: item[1], reverse=True))
    names = list(areas.keys())[:]

    fig, ax = plt.subplots(1, 2, figsize=(16, 16))
    ax1, ax2 = ax[0], ax[1]

    # Gráfico de linhas à esquerda (ax1) representando os perfis de desempenho
    for i in range(len(names)):
        ax1.plot(vet[names[i]], vet['h'], linestyle = linestyle_tuple[i % len(linestyle_tuple)][1], label=names[i])

    # Configurações do eixo ax1 (gráfico de linhas)
    ax1.set_title("Performance Profiles")                                      
    ax1.set_xlabel(fr'$\tau$')
    ax1.set_ylabel(fr'$\rho(\tau)$')
    ax1.set_xscale('log')
    ax1.legend()

    # Gráfico de barras à direita (ax2) representando as áreas normalizadas
    for i in range(len(names)):
        ax2.bar(names[i], areas[names[i]], alpha=0.3)
        # Adiciona o valor dentro da barra
        height = areas[names[i]]
        ax2.text(
            names[i],  # Posição x do texto (nome da barra)
            height / 2,  # Posição y do texto (metade da altura da barra)
            '{}'.format(height),  # Texto (valor da barra)
            ha='center',  # Alinhamento horizontal do texto
            va='center',  # Alinhamento vertical do texto
            color='black',  # Cor do texto
            fontsize=10,  # Tamanho da fonte
            rotation=90
        )

    # Configurações do eixo ax2 (gráfico de barras)
    ax2.set_title('Área')
    ax2.set_ylabel('Área')
    ax2.set_xticks(range(len(names)))
    ax2.set_xticklabels([names[i] for i in range(len(names))], fontsize=10, rotation=90)
    plt.xticks(rotation=90)

    # Ajusta automaticamente a disposição dos subplots para evitar sobreposição
    plt.tight_layout()
    # Exibe o gráfico
    plt.savefig(f"{algoritm_name}.svg")
    plt.show()

    return areas