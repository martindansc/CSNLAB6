#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <random>

using namespace std;

default_random_engine generator;

bool debug = false;

struct representation {
    representation(int size) {
        vertices = multiset<int>();
        degrees = vector<int>(size);
        n = 0;
    }

    multiset<int> vertices;
    vector<int> degrees;
    int n;
};

void add_inital(representation& out, int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size - 1; j++) {
            out.vertices.insert(i);
        }

        out.degrees[i] = size - 1;
    }

    out.n = size;
}

void slow_safe_add(representation& out, int n_edges) {
    vector<pair<int,int> > erased_vertices;

    for(int i = 0; i < n_edges; i++) {
       
        uniform_int_distribution<int> distribution(0, out.vertices.size());
        multiset<int>::iterator it = out.vertices.begin();
        advance(it, distribution(generator));

        int vertex = *it;
        int erased = out.vertices.erase(vertex);

        if(debug) cout << "Adding edge to vertex: " << vertex << endl;

        erased_vertices.push_back(pair<int, int> (vertex, erased));

        out.degrees[vertex]++;
    }

    for(int i = 0; i < erased_vertices.size(); i++) {
        pair<int,int> to_restore = erased_vertices[i];
        for(int j = 0; j < to_restore.second + 1; j++) {
            out.vertices.insert(to_restore.first);
        }
    }

    for(int i = 0; i < n_edges; i++) {
        out.vertices.insert(out.n);
    }

    out.degrees[out.n] = n_edges;
    out.n++;
}

void fast_risky_add(representation& out, int n_edges) {
    set<int> already_choosen = set<int>();

    uniform_int_distribution<int> distribution(0, out.vertices.size());

    int i = 0;
    while(i < n_edges) {
        multiset<int>::iterator it = out.vertices.begin();
        advance(it, distribution(generator));

        int vertex = *it;
        
        if(already_choosen.count(vertex) == 1) {
            continue;
        }

        if(debug) cout << "Adding edge to vertex: " << vertex << endl;


        already_choosen.insert(vertex);

        out.degrees[vertex]++;
        out.vertices.insert(vertex);
            
        i++;
    }

    for(int i = 0; i < n_edges; i++) {
        out.vertices.insert(out.n);
    }

    out.degrees[out.n] = n_edges;
    out.n++;
}

void add_vertex(representation& out, int n_edges) {
    fast_risky_add(out, n_edges);
}

void update_tracked(representation& in, vector<int>& tracking, int n, int t) {
    tracking[t - 1] = in.degrees[n - 1];
}

void output_vector(vector<int>& to_print, string filename) {
    ofstream output("out/" + filename + ".txt");
    bool first = true;
    for(int val : to_print) {
        
        if(!first) {
            output << " ";
        } else {
            first = false;
        }

        output << val;
    }
}

int main() {

    int s0 = 1000;
    int m0 = 10;
    int tm = 10000;

    int max_size = s0 + tm;

    representation rep(max_size);

    vector<int> tracked1 = vector<int>(tm);
    vector<int> tracked10 = vector<int>(tm);
    vector<int> tracked100 = vector<int>(tm);
    vector<int> tracked1000 = vector<int>(tm);

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
