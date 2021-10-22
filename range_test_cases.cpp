#include "bits/stdc++.h"
#include "ext/pb_ds/assoc_container.hpp"
#include "ext/pb_ds/tree_policy.hpp"

using namespace std;
using namespace __gnu_pbds;


#define ll long long int
#define pb push_back
#define mp make_pair
#define ff first
#define ss second
#define all(a) a.begin(),a.end()

typedef tree <ll, null_type, less <ll>, rb_tree_tag, tree_order_statistics_node_update> ordered_set;
// order_of_key(val): returns the number of values less than val
// find_by_order(k): find_by_order(0) returns an iterator to the smallest element
int main(void)
{
    srand(time(NULL));
    vector<int> v(100);
    iota(all(v), 1);
    random_shuffle(all(v));
    for(int u: v) {
        cout << "INSERT " << u << endl;
    }
    for(int i = 1; i < 100; i++) {
        cout << "RANGE " << i << " " << i + 1 << endl;
    }
}