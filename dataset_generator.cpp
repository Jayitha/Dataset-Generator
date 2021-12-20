#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <vector> 
#include <string>
#include <fstream>
#include <cmath>
#include <random>

using namespace std;

double min(double a, double b) {
    if (a < b) {
        return a;
    }
    return b;
}

double max(double a, double b) {
    if (a > b) {
        return a;
    }
    return b;
}

int CARDINALITY = 1000;
int DIMENSIONALITY = 2;
double DATA_MIN = 0;
double DATA_MAX = 1;

vector<vector<double>> raw_data;
vector<vector<double>> cor_matrix;
vector<double> mean_vector;
vector<double> min_vector;
vector<double> max_vector;
vector<double> std_vector;

enum DataType{
    ANTI,
    INDEP,
    CORR
};

DataType data_type = CORR;

string etostr(DataType& type){
    switch(type){
        case ANTI: return "ANTI";
        case INDEP: return "INDEP";
        case CORR: return "CORR";
        default: return "CORR";
    }
}

DataType strtoe(string type){
    if(type == "ANTI")
        return ANTI;
    if(type == "INDEP")
        return INDEP;
    else
        return CORR;
}

void anti_generator(){
    random_device rd;
    mt19937 e2(rd());
    std::normal_distribution<> plane_dist(0.5, 0.003);
    for(int i = 0; i < CARDINALITY; i++){
        double plane_point = 0;
        do{
            plane_point = plane_dist(e2);
        }while(plane_point < 0 || plane_point > 1);
        std::default_random_engine generator;
        double diff = min(plane_point, 1 - plane_point);
        double min_r = max(0, plane_point - diff/sqrt(2));
        double max_r = min(1, plane_point + diff/sqrt(2));
        if(plane_point <= 0.5){
            min_r = 0;
            max_r = plane_point + diff/sqrt(2);
        }
        else {
            max_r = 1;
            min_r = plane_point - diff/sqrt(2);
        }
        // std::uniform_real_distribution<double> distribution(min_r,max_r);
        std::uniform_real_distribution<double> distribution(0,1);
        do{
            for(int j = 0; j < DIMENSIONALITY; j++){
                raw_data[i][j] = distribution(generator);
            }
        }while(abs(accumulate(raw_data[i].begin(), raw_data[i].end(), 0.0) - DIMENSIONALITY*plane_point) > 0.0001);
    }
    return;
}

void indep_generator(){
    for(int i = 0; i < CARDINALITY; i++){
        for(int j = 0; j < DIMENSIONALITY; j++){
            raw_data[i][j] = rand()/double(RAND_MAX);
        }
    }
}

void corr_generator(){
    random_device rd;
    mt19937 e2(rd());
    std::normal_distribution<> plane_dist(0.5, 0.2);
    for(int i = 0; i < CARDINALITY; i++){
        double plane_point = 0;
        do{
            plane_point = plane_dist(e2);
        }while(plane_point < 0 || plane_point > 1);
        std::normal_distribution<> point_dist(plane_point, 0.01);
        for(int j = 0; j < DIMENSIONALITY; j++){
            do{
                raw_data[i][j] = point_dist(e2);
            }while(raw_data[i][j] < 0 || raw_data[i][j] > 1);
        }
    }
    return;

}

void save_to_disk(){
    string file_name = etostr(data_type) + "_" + to_string(DIMENSIONALITY) + "_" + to_string(CARDINALITY);
    ofstream fout("./data/" + file_name + ".txt", ios::out);
    ofstream fout_csv("./data/" + file_name + ".csv", ios::out);
    fout << DIMENSIONALITY <<  endl;
    fout << CARDINALITY << endl;
    fout_csv << "ID"; 
    for(int i = 0; i < DIMENSIONALITY; i++){
        fout_csv << ";d" << i+1;
    }
    fout_csv << endl;
    for(int i = 0; i < CARDINALITY; i++){
        fout_csv << i+1;
        for(int j = 0; j < DIMENSIONALITY; j++){
            fout << raw_data[i][j] << " ";
            fout_csv << ";" << raw_data[i][j];
        }
        fout << endl;
        fout_csv << endl;
    }
    fout.close();
    fout_csv.close();
}

void min_max_mean_vectors(){
    for(int i = 0; i < CARDINALITY; i++){
        for(int j = 0; j < DIMENSIONALITY; j++){
            min_vector[j] = min(min_vector[j], raw_data[i][j]);
            max_vector[j] = max(max_vector[j], raw_data[i][j]);
            mean_vector[j] = (i*(mean_vector[j]) + raw_data[i][j])/(i+1);
            std_vector[j] = (i*(std_vector[j]) + pow(raw_data[i][j], 2))/(i+1);
            if(i == CARDINALITY - 1)
                std_vector[j] = sqrt(std_vector[j] - pow(mean_vector[j], 2));
        }
    }
}

void correlation_matrix(){
    for(int i = 0; i < CARDINALITY; i++){
        for(int j1 = 0; j1 < DIMENSIONALITY; j1++){
            for(int j2 = 0; j2 < DIMENSIONALITY; j2++){
                cor_matrix[j1][j2] = ((i * cor_matrix[j1][j2]) + (raw_data[i][j1] - mean_vector[j1]) * (raw_data[i][j2] - mean_vector[j2]))/(i+1);
                if(i == CARDINALITY - 1){
                    cor_matrix[j1][j2] = cor_matrix[j1][j2]/(std_vector[j1] * std_vector[j2]);
                }
            }
        }
    }
}

void summary_stats(){
    string file_name = etostr(data_type) + "_" + to_string(DIMENSIONALITY) + "_" + to_string(CARDINALITY) + "_summary.md";
    ofstream fout("./data/" + file_name, ios::out);
    fout << "## Summary" << endl;
    fout << "**TYPE:** " << etostr(data_type) << "  " << endl;
    fout << "**Dimensionality:** " << DIMENSIONALITY << "  " << endl;
    fout << "**Cardinality:** " << CARDINALITY << "  " << endl << endl;
    min_max_mean_vectors();
    fout << "| Dimension | Min | Mean | Max | Std Dev |" << endl;
    fout << "| :-------- | :--: | :--: | :--: | :--: |" << endl;
    for(int i = 0; i < DIMENSIONALITY; i++){
        fout << "| d" << i+1 << " | " << min_vector[i] << " | " << mean_vector[i] << " | " << max_vector[i] << " | " << std_vector[i] << " |" << endl;
    }
    fout << endl << endl;
    correlation_matrix();
    fout << "### Correlation Matrix  " << endl;
    fout << "| - | ";
    for(int i = 0; i < DIMENSIONALITY; i++){
        fout << "d" << i+1 << " | ";
    }
    fout << endl;
    fout << "| :--: | ";
    for(int i = 0; i < DIMENSIONALITY; i++){
        fout << ":--: | ";
    }
    fout << endl;
    for(int i = 0; i < DIMENSIONALITY; i++){
        fout << "| d" << i+1 << " | ";
        for(int j = 0; j < DIMENSIONALITY; j++){
            fout << cor_matrix[i][j] << " | ";
        }
        fout << endl;
    }
    fout.close();
}

void generator(){
    switch(data_type){
        case ANTI: anti_generator(); break;
        case INDEP: indep_generator(); break;
        case CORR: corr_generator(); break;
        default: return;
    }
    save_to_disk();
    summary_stats();
}

int main(int argc, char* argv[] ){
    data_type = strtoe(argv[1]);
    DIMENSIONALITY = atoi(argv[2]);
    CARDINALITY = atoi(argv[3]);
    raw_data.assign(CARDINALITY, vector<double>(DIMENSIONALITY, 0));
    cor_matrix.assign(DIMENSIONALITY, vector<double>(DIMENSIONALITY, 0));
    mean_vector.assign(DIMENSIONALITY, 0);
    min_vector.assign(DIMENSIONALITY, DATA_MAX);
    max_vector.assign(DIMENSIONALITY, DATA_MIN);
    std_vector.assign(DIMENSIONALITY, 0);
    srand(time(nullptr));
    generator();
}