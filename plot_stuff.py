def n_word(string, n):
    first_word = string.split(' ')[n]
    return first_word

import re
import pandas as pd

def parse_file(filepath):
    """
    Parse text at given filepath

    Parameters
    ----------
    filepath : str
        Filepath for file_object to be parsed

    Returns
    -------
    data : pd.DataFrame
        Parsed data

    """
    
    instance_name = 0
    remorques = []  # create an empty list to collect the data
    stations = []
    # open the file and read through it line by line
    with open(filepath, 'r') as file_object:
        line = file_object.readline()
        while line:
            
            # pretraitement
            print("line is", line)
            line = ' '.join(line.split())
            
            if re.match(r'^\s*$', line):
                line = file_object.readline()
                continue
            
            if (line[0] == '#') | (n_word(line, 0) == 'version'):
                print("line ignored")
                
            if (n_word(line, 0) == "velib"):
                instance_name = n_word(line, 1)
                print("got instance name")
            
            if (n_word(line, 0) == "remorque"):
                remorque_temp = {}
                remorque_temp["id"] = n_word(line, 1)
                remorque_temp["x"] = n_word(line, 2)
                remorque_temp["y"] = n_word(line, 3)
                remorque_temp["K"] = n_word(line, 4)
                remorques.append(remorque_temp)
                print("got remorque")

            if (n_word(line, 0) == "station"):
                station_temp = {}
                station_temp["id"] = n_word(line, 1)
                station_temp["x"] = n_word(line, 2)
                station_temp["y"] = n_word(line, 3)
                station_temp["capa"] = n_word(line, 4)
                station_temp["ideal"] = n_word(line, 5)
                station_temp["nbvp"] = n_word(line, 6)
                stations.append(station_temp)
                print("got remorque")

            line = file_object.readline()

    return instance_name, pd.DataFrame(remorques), pd.DataFrame(stations)

import matplotlib.pyplot as plt
import matplotlib

def plot_input_from_df(stations_df_original, remorques_df_original, circuits):
    #init plot
    stations_df = stations_df_original.copy()
    remorques_df = remorques_df_original.copy()
    
    # get coordinates of all circuits and add it to the plot
    remorque_index=0
    stations_df = stations_df.set_index("id")
    stations_df["x"] = pd.to_numeric(stations_df["x"])
    stations_df["y"] = pd.to_numeric(stations_df["y"])
    remorques_df["x"] = pd.to_numeric(remorques_df["x"])
    remorques_df["y"] = pd.to_numeric(remorques_df["y"])
    for circuit in circuits:
        xs_ordered = [remorques_df.loc[remorque_index, "x"]]
        ys_ordered = [remorques_df.loc[remorque_index, "y"]]
        xs_ordered+=(list(stations_df.loc[circuit, "x"].values))
        ys_ordered+=(list(stations_df.loc[circuit, "y"].values))
        xs_ordered.append(remorques_df.loc[remorque_index, "x"])
        ys_ordered.append(remorques_df.loc[remorque_index, "y"])
        print(xs_ordered, ys_ordered)
        plt.plot(list(xs_ordered), list(ys_ordered))
        remorque_index+=1

    plt.scatter(stations_df["x"], stations_df["y"], c="blue", label="Station")
    plt.scatter(remorques_df["x"], remorques_df["y"], c="red", label="Entrepot", marker="s")
    plt.grid()
    plt.legend()

    plt.show()
    return None

import re
import pandas as pd

def parse_solution(filepath):
    """
    Parse text at given filepath

    Parameters
    ----------
    filepath : str
        Filepath for file_object to be parsed

    Returns
    -------
    data : pd.DataFrame
        Parsed data

    """
    
    instance_name = 0
    stats = []  # create an empty list to collect the data
    circuits = []
    in_circuit = False
    # open the file and read through it line by line
    with open(filepath, 'r') as file_object:
        line = file_object.readline()
        while line:
            
            # pretraitement
            print("line is", line)
            line = ' '.join(line.split())
            
            if re.match(r'^\s*$', line):
                line = file_object.readline()
                continue
                
            if (n_word(line, 0) != "circuit") and not in_circuit:
                line = file_object.readline()
                print("useless line")
                continue
            
            if (n_word(line, 0) == "circuit"):
                stats_temp = {}
                stats_temp["id"] = n_word(line, 1)
                stats_temp["charge_init"] = n_word(line, 2)
                stats_temp["desequ"] = n_word(line, 3)
                stats_temp["longueur"] = n_word(line, 4)
                stats.append(stats_temp)
                in_circuit = True
                print("got stat")
                line = file_object.readline()
                continue

            if in_circuit and (n_word(line, 0) != "end"):
                print("in circuit")
                circuit_temp=[]
                while (n_word(line, 0) != "end"):
                    circuit_temp.append(n_word(line, 0))
                    print("    got circuit step")
                    line = file_object.readline()
                    line = ' '.join(line.split())
                    continue
                circuits.append(circuit_temp)
                print("out of circuit")
                continue
                
            if (n_word(line, 0)=="end"):
                in_circuit=False
                print("end of circuit confirmed")
                line = file_object.readline()
                continue 
                    
    return stats, circuits

instance_name, remorques, stations = parse_file('../data/v2.dat')
stats, circuits = parse_solution("../build/v2-2-581.sol")

plot_input_from_df(stations, remorques, circuits)
