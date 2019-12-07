#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <random>

using namespace std;

default_random_engine generator;

bool debug = true;

struct representation {
    representation(int size) {
        vertices = vector<int>();
        degrees = vector<int>(size);
        n = 0;
    }

    vector<int> vertices;
    vector<int> degrees;
    int n;
};

void output_vector(vector<int>& to_print, string filename) {
    ofstream output("out/" + filename + ".txt");
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

    cout << endl;
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
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size - 1; j++) {
            out.vertices.push_back(i);
        }

        out.degrees[i] = size - 1;
    }

    out.n = size;

    if(debug) {
        cout << "Performing initial check" << endl;
        check_consistency(out);
    }
}

void add_inital_grid(representation& out, int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size - 1; j++) {
            out.vertices.push_back(i);
        }

        out.degrees[i] = size - 1;
    }

    out.n = size;

    if(debug) {
        cout << "Performing initial check" << endl;
        check_consistency(out);
    }
}

void add_inital(representation& out, int size) {
    add_inital_complete(out, size);
}

void add_vertex(representation& out, int n_edges) {
    set<int> already_choosen = set<int>();

    uniform_int_distribution<int> distribution(0, out.vertices.size());

    int i = 0;
    while(i < n_edges) {
        vector<int>::iterator it = out.vertices.begin();
        advance(it, distribution(generator));

        int vertex = *it;
        
        if(already_choosen.count(vertex) == 1) {
            continue;
        }

        if(debug) cout << "Adding edge to vertex: " << vertex << endl;


        already_choosen.insert(vertex);

        out.degrees[vertex]++;
        out.vertices.push_back(vertex);

        if(debug) cout << "New vertex degree: " << out.degrees[vertex] << endl; 
            
        i++;
    }

    for(int j = 0; j < n_edges; j++) {
        out.vertices.push_back(out.n);
    }

    out.degrees[out.n] = n_edges;
    out.n++;

    if(debug) {
        cout << "Current vertex vector" << endl;
        output_vector(out.vertices, "tracked1");
        check_consistency(out);
    }
}

void update_tracked(representation& in, vector<int>& tracking, int n, int t) {
    tracking[t - 1] = in.degrees[n - 1];
}

int main() {

    int s0 = 10;
    int m0 = 10;
    int tm = 10;

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
        cout << "Add t: " << t << endl;
        add_vertex(rep, m0);

        update_tracked(rep, tracked1, s0 + 1, t);
        update_tracked(rep, tracked10, s0 + 10, t);
        update_tracked(rep, tracked100, s0 + 100, t);
        update_tracked(rep, tracked1000, s0 + 1000, t);

    }

    output_vector(rep.degrees, "degreees");
    output_vector(tracked1, "tracked1");
    output_vector(tracked10, "tracked10");
    output_vector(tracked100, "tracked100");
    output_vector(tracked1000, "tracked1000");
    
}
