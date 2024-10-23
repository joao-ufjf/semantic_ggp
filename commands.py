import os
import shutil
import numpy as np
import sys

crossovers = {
    "DEF" : 0,
    "RSC" : 1,
    "FVSC": 2,
    "SAC" : 3,
    "SSC" : 4,
    "SCPS": 5 
}

distances = {
    "ABS":  0,
    "COS":  1
}

amounts = [
    50, 
    100, 
    500, 
    1000
]

noises = {
    "original": 0,
    "noise"   : 1
}

problems = {
    2 : ["f1", "f2", "f5", "f6", "f7", "f8", "f9", "f10", "f11"],
    3 : ["f4"],
    5 : ["f3", "f13", "f15", "f16", "f17", "f18", "f19", "f21"],
    6 : ["f14"],
    10: ["f12", "f20"]
}

grammars = {
    2  : "grammar_2_fp",
    3  : "grammar_3_fp",
    5  : "grammar_5_fp",
    6  : "grammar_6_fp",
    10 : "grammar_10_fp"
}

abs_points = [10 ** x for x in range(-5, 2, 1)] # np.linspace(lowest, uppest, points, endpoint=True),,
cos_points = [10 ** x for x in range(-5, 0, 1)] # np.linspace(lowest, uppest, points, endpoint=True),,

lower_bounds = {
    "ABS": {
        "DEF" : [0],
        "RSC" : abs_points,
        "FVSC": abs_points,
        "SAC" : abs_points,
        "SSC" : abs_points,
        "SCPS": [0],
    },
    "COS": {
        "DEF" : [0],
        "RSC" : cos_points,
        "FVSC": cos_points,
        "SAC" : cos_points,
        "SSC" : cos_points,
        "SCPS": [0],
    },
}

upper_bounds = {
    "ABS": {
        "DEF" : [1],
        "RSC" : abs_points,
        "FVSC": abs_points,
        "SAC" : abs_points,
        "SSC" : abs_points,
        "SCPS": [1],
    },
    "COS": {
        "DEF" : [1],
        "RSC" : cos_points,
        "FVSC": cos_points,
        "SAC" : cos_points,
        "SSC" : cos_points,
        "SCPS": [1],
    },
}

base = "input/nicolau/fs/results"

def makedir(path):
    try:
        if os.path.exists(path):
            if os.path.isdir(path):
                shutil.rmtree(path)
                print(f"A pasta '{path}' foi excluída com sucesso.")
            else:
                print(f"'{path}' não é uma pasta.")
        
        os.mkdir(path)
        # print(f"Pasta '{path}' criada.")
    except Exception as e:
        print(f"Erro ao excluir a pasta '{path}': {e}")

# Adicione aqui a leitura dos parâmetros start e end
if len(sys.argv) != 3:
    print("Uso: script.py <start> <end>")
    sys.exit(1)

start = int(sys.argv[1])
end = int(sys.argv[2])

makedir(base)

for n_var in problems:
    n_problems = problems[n_var]
    for problem in n_problems:
        makedir("{}/{}".format(base, problem))
        for amount in amounts:
            makedir("{}/{}/{}".format(base, problem, amount))
            for noise in noises:
                makedir("{}/{}/{}/{}".format(base, problem, amount, noise))
                for distance in distances:
                    makedir("{}/{}/{}/{}/{}".format(base, problem, amount, noise, distance))
                    for crossover in crossovers:
                        makedir("{}/{}/{}/{}/{}/{}".format(base, problem, amount, noise, distance, crossover))
                        for lower_bound in lower_bounds[distance][crossover]:
                            makedir("{}/{}/{}/{}/{}/{}/{}".format(base, problem, amount, noise, distance, crossover, '%.2E' % lower_bound))
                            for upper_bound in upper_bounds[distance][crossover]:
                                if lower_bound < upper_bound:
                                    makedir("{}/{}/{}/{}/{}/{}/{}/{}".format(base, problem, amount, noise, distance, crossover, '%.2E' % lower_bound, '%.2E' % upper_bound))
                                    # with open("{}/{}/{}/{}/{}/{}/{}/{}/res.csv".format(base, problem, amount, noise, distance, crossover, lower_bound, upper_bound), 'w') as file:
                                    #     file.write('seed;time;training;test;validation;model;constants')
                                    
                                    # print("{}/{}/{}/{}/{}/{}/{}/{}".format(base, problem, amount, noise, distance, crossover, lower_bound, upper_bound))

commands = []

for amount in amounts:
    for seed in range(start, end):
        for distance in distances:
            for n_var in problems:
                n_problems = problems[n_var]
                for problem in n_problems:
                    for noise in noises:
                        for crossover in crossovers:
                            for lower_bound in lower_bounds[distance][crossover]:
                                for upper_bound in upper_bounds[distance][crossover]:
                                    if lower_bound < upper_bound:
                                        commands.append("{} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}".format(
                                            "./bin/Release/gpufjf",
                                            seed, 
                                            "input/nicolau/grammars/{}".format(grammars[n_var]),
                                            "input/nicolau/fs/{}/{}/{}/data".format(problem, amount, noise),
                                            "input/nicolau/fs/groups/{}".format(amount),
                                            0,
                                            "input/nicolau/fs/results/{}/{}/{}/{}/{}/{}/{}/".format(problem, amount, noise, distance, crossover, '%.2E' % lower_bound, '%.2E' % upper_bound),
                                            10, 0, 0, 0, 0, 0, 
                                            500, #popSize
                                            "{}".format(crossovers[crossover]),
                                            0,
                                            "{}".format('%.2E' % lower_bound),
                                            "{}".format('%.2E' % upper_bound),
                                            "{}".format(distances[distance]))
                                            )

try:
    os.remove("commands.txt")
    print('os.remove("commands.txt")')
except:
    print('ainda não há comandos')

with open('commands.txt', 'w') as file:
    for command in commands[:]:
        file.write(command + '\n')

with open('executed.txt', 'w') as file:
    file.write('')
