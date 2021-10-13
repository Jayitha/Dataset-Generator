import numpy as np
from scipy.linalg import null_space
import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt
import sys
import os
from pathlib import Path
sns.set_theme(style="white")

DATA_PATH = "./data/"

n = len(sys.argv)
type = sys.argv[1]

if not type in ["INDEP", "CORR", "ANTI"]:
    quit()

dimensionality = int(sys.argv[2])
cardinality = int(sys.argv[3])
epsilon = 0.1
if n > 4:
    epsilon = float(sys.argv[4])
directory_name = type + "_" + sys.argv[2] + "_" + sys.argv[3] + "_" + str(epsilon)
Path(DATA_PATH + directory_name).mkdir(parents=True, exist_ok=True)

dataset = np.zeros((cardinality, dimensionality))
skyline_indices = []
non_skyline_indices = []
skyline_status = []

A = np.ones((1, dimensionality))
null_A = np.zeros((dimensionality, dimensionality - 1))
for i in range(dimensionality - 1):
    null_A[0,i] = -1
    null_A[i+1,i] = 1

def dominates(idx1, idx2):
    point1 = dataset[idx1,:]
    point2 = dataset[idx2,:]
    one_dom_two = False
    two_dom_one = False
    for i in range(dimensionality):
        if(point1[i] < point2[i]):
            one_dom_two = True
        if(point2[i] < point1[i]):
            two_dom_one = True
    if(one_dom_two and not two_dom_one):
        return True
    return False

def skyline():
    skyline_indices.clear()
    non_skyline_indices.clear()
    for i in range(cardinality):
        dominated = False
        j = 0
        while(j < len(skyline_indices)):
            if(dominates(i,skyline_indices[j])):
                del skyline_indices[j]
            elif(dominates(skyline_indices[j],i)):
                dominated = True
                break
            else:
                j = j + 1
        if(not dominated):
            skyline_indices.append(i)
        else:
            non_skyline_indices.append(i)

def valid(idx):
    point = dataset[idx,:]
    if(np.any((point < 0)|(point > 1 ))):
        return False
    return True

def data_frame():
    data = {}
    for i in range(dimensionality):
        data["D_" + str(i+1)] = list(dataset[:,i])
    skyline_status = ["non_skyline" for x in range(cardinality)]
    for i in skyline_indices:
        skyline_status[i] = "skyline"
    data["skyline_status"] = skyline_status
    df = pd.DataFrame(data=data)
    return df

def save_data():
    skyline()
    print("# Skyline Points: ", len(skyline_indices))
    df = data_frame()
    sns.heatmap(df.loc[:, df.columns != 'skyline_status'].corr())
    plt.savefig(DATA_PATH + directory_name + '/heatmap.eps')
    sns.pairplot(df, hue="skyline_status", palette="dark")
    plt.savefig(DATA_PATH + directory_name + '/scatterplot_matrix.eps')
    f = open(DATA_PATH + directory_name+ "/" + directory_name+  ".txt", "w")
    f.write(str(dimensionality) + "\n")
    f.write(str(cardinality) + "\n")
    f.write(str(epsilon) + "\n")
    for i in range(cardinality):
        for j in range(dimensionality):
            f.write(str(dataset[i,j]) + " ")
        f.write("\n")
    f.close()

def independent_dataset_generator():
    for i in range(cardinality):
        for j in range(dimensionality):
            dataset[i,j] = np.random.uniform(0,1)  

def correlated_dataset_generator():
    i = 0
    while(i < cardinality):
        plane_point = -1
        while(plane_point < 0 or plane_point > 1):
            plane_point = np.random.normal(0.5, 0.3)
        dataset[i,:] = np.random.normal(plane_point, 0.125, (1, dimensionality))
        if valid(i):
            i = i + 1

def anti_correlated_dataset_generator():
    i = 0
    while(i < cardinality):
        plane_point = -1
        while(plane_point < 0 or plane_point > 1):
            plane_point = np.random.normal(0.5, 0.03)
        dataset[i,:] = (plane_point * np.ones((dimensionality, 1)) + np.dot(null_A, np.random.uniform(0, 1, (dimensionality-1, 1)))).T
        if valid(i):
            i = i + 1

if type == "INDEP":
    independent_dataset_generator()
elif type == "CORR":
    correlated_dataset_generator()
elif type == "ANTI":
    anti_correlated_dataset_generator()

save_data()
    