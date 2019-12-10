#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <random>

using namespace std;

random_device rd;
mt19937 generator(rd());

bool debug = false;

string alg;

struct representation {
    representation(int size) {
        vertices = vector<int>();
        degrees = vector<int>(size);
        edges = vector<vector<bool> >(size, vector<bool>(size, false));
        n = 0;
    }

    vector<int> vertices;
    vector<int> degrees;
    vector< vector<bool> > edges;
    int n;
};

void output_vector(vector<int>& to_print, string filename) {
    ofstream output("out/" + alg + "/" + filename + ".txt");
    bool first = true;
    for(int val : to_print) {
        
        if(!first) {
            output << " ";
            if(debug) cout << " ";
        } else {
            first = false;
        }

        output << val;
        if(debug) cout << val;
    }

    if(debug) cout << endl;
}

void add_edge(representation& out, int a, int b) {

    if(out.edges[a][b]) {
        cout << "ERROR!!! Edge already exists." << endl;
    }
    out.vertices.push_back(a);
    out.vertices.push_back(b);

    out.degrees[a]++;
    out.degrees[b]++;

    out.edges[a][b] = true;
    out.edges[b][a] = true;
}

int insert_new_vertex(representation& out) {
    out.n++;
    return out.n - 1;
}

void insert_n_vertex(representation& out, int n) {
    out.n += n;
}

bool exists_edge(representation& out, int a, int b) {
    return out.edges[a][b];
}

void check_consistency(representation& rep) {
    for(int i = 0; i < (int) rep.degrees.size(); i++) {

        if(rep.n < i) {
            if(rep.degrees[i] != 0) {
                cout << "!!!FALSE!!!!" << endl;
            }
        }

        int cnt = count(rep.vertices.begin(), rep.vertices.end(), i);
        if(cnt != rep.degrees[i]) {
            cout << "!!!FALSE!!!!";
        }
    }
}

void add_inital_complete(representation& out, int size) {

    insert_n_vertex(out, size);

    for(int i = 0; i < size; i++) {
        for(int j = i + 1; j < size; j++) {
            add_edge(out, i, j);
        }
    }

    if(debug) {
        cout << "Performing initial check" << endl;
        check_consistency(out);
    }
}

void add_inital_grid(representation& out, int size) {
    
    insert_n_vertex(out, size);

    for(int i = 0; i < size; i++) {
        add_edge(out, i, (i + 1) % size);
    }

    if(debug) {
        cout << "Performing initial check" << endl;
        check_consistency(out);
    }
}

void add_inital(representation& out, int size) {
    add_inital_grid(out, size);
}

void add_vertex(representation& out, int n_edges) {
    uniform_int_distribution<int> distribution;
    if(alg == "BA_RA") distribution = uniform_int_distribution<int>(0, out.n);
    else distribution = uniform_int_distribution<int>(0, out.vertices.size() + 1);

    int new_vertex = insert_new_vertex(out);

    int i = 0;
    while(i < n_edges) {

        int vertex;
        if(alg == "BA_RA") vertex = distribution(generator);
        else vertex = out.vertices[distribution(generator)];
        
        if(exists_edge(out, vertex, new_vertex)) continue;

        if(debug) cout << "Adding edge to vertex: " << vertex << endl;

        add_edge(out, vertex, new_vertex);

        if(debug) cout << "New vertex degree: " << out.degrees[vertex] << endl; 
            
        i++;
    }

    if(debug) {
        cout << "Current vertex vector" << endl;
        output_vector(out.vertices, alg + "tracked1");
        check_consistency(out);
    }
}

void add_random_edges(representation& out, int n_edges) {

    // select first vertex
    uniform_int_distribution<int> distribution(0, out.n);

    bool found = false;
    int vertex;
    while(!found) {
        vertex = distribution(generator);
        if(out.n - out.degrees[vertex] > n_edges) {
            found = true;
        }
    }

    // add edges to selected vertex
    int i = 0;
    while(i < n_edges) {
        int vertex_2 = distribution(generator);

        if(exists_edge(out, vertex, vertex_2)) continue;
       
        if(debug) cout << "Adding edge to vertex: " << vertex << endl;

        add_edge(out, vertex, vertex_2);

        if(debug) cout << "New vertexs degrees: " << out.degrees[vertex] << " " << out.degrees[vertex_2] << endl; 
            
        i++;
    }
}

void update_tracked(representation& in, vector<int>& tracking, int n, int t) {
    tracking[t - 1] = in.degrees[n - 1];
}

int main(int argc, char* argv[]) {

    alg = argc > 1 ? (string) argv[1] : "BA";
    int s0 = argc > 2 ? stoi(argv[2]) : 20;
    int m0 = argc > 3 ? stoi(argv[3]) : 10;
    int tm = argc > 4 ? stoi(argv[4]) : 100000;

    if(argc > 5) {
        string debug_arg = argv[5]; 
        debug = (debug_arg == "T" || debug_arg == "true");
    }

    int max_size = s0 + tm;

    representation rep(max_size);

    vector<int> tracked1 = vector<int>(tm, 0);
    vector<int> tracked10 = vector<int>(tm, 0);
    vector<int> tracked100 = vector<int>(tm, 0);
    vector<int> tracked1000 = vector<int>(tm, 0);

    add_inital(rep, s0);

    if(debug) cout << "Added initial" << endl;

    // add one vertex each t
    for(int t = 1; t <= tm; t++) {

        cout << "Time: " << t << endl;

        if(alg == "BA" || alg == "BA_RA") {
            add_vertex(rep, m0);

            update_tracked(rep, tracked1, s0 + 1, t);
            update_tracked(rep, tracked10, s0 + 10, t);
            update_tracked(rep, tracked100, s0 + 100, t);
            update_tracked(rep, tracked1000, s0 + 1000, t);
        }
        else if(alg == "BA_NG") {
            add_random_edges(rep, m0);

            update_tracked(rep, tracked1, 1, t);
            update_tracked(rep, tracked10, 10, t);
            update_tracked(rep, tracked100, 100, t);
            update_tracked(rep, tracked1000, 1000, t);
        }
        else {
            cout << "Invalid algorithm selected" << endl;
            return 1;
        }

    }

    output_vector(rep.degrees, "degreees");
    output_vector(tracked1, "tracked1");
    output_vector(tracked10, "tracked10");
    output_vector(tracked100, "tracked100");
    output_vector(tracked1000, "tracked1000");
    
}
