/*
 * 10 Days Over Kanpur
 * Submission: Hall 5 - Nawabs
*/

#include <bits/stdc++.h>
using namespace std;

/* ---------------- handy macros & helpers ---------------- */
using ll = long long;
using ull = unsigned long long;
using ld = long double;
using pii = pair<int, int>;
using pll = pair<ll, ll>;
using vi = vector<int>;
using vll = vector<ll>;
using vvi = vector<vi>;
using vvll = vector<vll>;
using vs = vector<string>;
using vpii = vector<pii>;
using vpll = vector<pll>;
using mii = map<int, int>;
using mll = map<ll, ll>;
#define all(x) begin(x), end(x)
#define rall(x) rbegin(x), rend(x)
#define ls(u) (u << 1)
#define rs(u) (u << 1 | 1)
#define rep(i, a, b) for (ll i = a; i <= b; i++)
#define rrep(i, a, b) for (ll i = a; i >= b; i--)
template <class T> inline bool ckmin(T &a, const T &b) {
  return b < a ? (a = b, true) : false;
}
template <class T> inline bool ckmax(T &a, const T &b) {
  return b > a ? (a = b, true) : false;
}

/* ------------- main solve function ------------- */
ll W, D, C, M, P, K, T, Bmax, R, N;
vvll prof;
vpll profLoc;
vvll drone;
vpll droneLoc;
vll battery(D);
vvll depot;
vpll depotLoc;
vvll inv;
vvll charge;
vpll chargeLoc;
vvll orders;
vll package;
vpll profNearCharge;
vpll depotNearCharge;
map<pll, ll> chargeID;

/* Input function --> Helper fucntion which takes the input in the following
format :

1) W D C M P K Tmax Bmax R - Grid size, Number of Drones, Professors, Depots,
Charging Stations, Item Types, Max Turns, Max Battery, and Charging Rate,
respectively. 2) Next D lines: The initial state of each drone d (from 0 to
D-1), with each line containing three space-separated integers: rd cd Bd
(initial row, column, and battery level). 3) Next C lines: The coordinates of
each professor c (from 0 to C-1), with each line containing two space-separated
integers: crc ccc (the cordinates of each professor). 4) Next M lines: The
coordinates of each depot m (from 0 to M-1), with each line containing two
space-separated integers: mrm mcm (the cordinates of each depot). 5) Next M
lines: The initial inventory for each depot m (from 0 to M-1). Each line
contains K space-separated integers: Invm,0 Invm,1. .. Invm,K
6) Next P lines: The coordinates of each charging station p (from 0 to P-1),
with each line containing two space-separated integers: prp pcp (the cordinates
of each charging station). 7) The final N lines: The complete list of all
delivery orders for the simulation. Each line represents one order i (from 0 to
N-1) and contains six space-separated integers: item_typei, origin_depoti,
destination_professori, appear_turni, deadline_turni valuei.

*/
void input() {
  cin >> W >> D >> C >> M >> P >> K >> T >> Bmax >> R;
  prof.assign(W, vll(W, -1));
  drone.assign(W, vll(W, -1));
  battery.assign(D, 0);
  depot.assign(W, vll(W, -1));
  droneLoc.assign(D, {0, 0});
  profLoc.assign(C, {0, 0});
  depotLoc.assign(M, {0, 0});
  inv.assign(M, vll(K, 0));
  charge.assign(W, vll(W, -1));
  package.assign(D, -1);
  chargeLoc.assign(P, {0, 0});
  profNearCharge.assign(C, {-1, -1});
  depotNearCharge.assign(M, {-1, -1});
  rep(i, 0, D - 1) {
    ll r, c, b;
    cin >> r >> c >> b;
    drone[r][c] = i;
    battery[i] = b;
    droneLoc[i] = {r, c};
  }
  rep(i, 0, C - 1) {
    ll r, c;
    cin >> r >> c;
    prof[r][c] = i;
    profLoc[i] = {r, c};
  }
  rep(i, 0, M - 1) {
    ll r, c;
    cin >> r >> c;
    depot[r][c] = i;
    depotLoc[i] = {r, c};
  }
  rep(i, 0, M - 1) {
    rep(j, 0, K - 1) {
      ll x;
      cin >> x;
      inv[i][j] = x;
    }
  }
  rep(i, 0, P - 1) {
    ll r, c;
    cin >> r >> c;
    charge[r][c] = i;
    chargeLoc[i] = {r, c};
  }
  // Orders
  // Take input until empty
  while (true) {
    ll item_type, origin_depot, destination_professor, appear_turn,
        deadline_turn, value;
    if (!(cin >> item_type >> origin_depot >> destination_professor >>
          appear_turn >> deadline_turn >> value))
      break;
    orders.push_back({item_type, origin_depot, destination_professor,
                      appear_turn, deadline_turn, value});
  }
  N = orders.size();
}

// Passwords for each order which was decrypted from the input file
vs passwords = {
  "horizon92", "zenith617", "yarrow32", "obelisk7053",
  "dagger27", "lattice23", "nebula54793", "obelisk4985",
  "zephyr54", "zephyr43239", "amber97707", "quantum69715",
  "zenith4286", "anchor92738", "dagger170", "cipher8009",
  "raptor3140", "willow303", "linchpin476", "sentinel415",
  "horizon17668", "paradox13", "yonder71206", "monolith884",
  "packet39", "cipher97990", "cursor3726", "yarrow54",
  "infosec21285", "jaguar512", "infosec7302", "debugger4764",
  "beacon490", "raven5288", "delta6979", "anchor459",
  "whisper68", "sentinel56995", "raven65", "nebula70",
  "canvas33", "xenon556", "whisper4791", "lattice80662",
  "binary71929", "granite81942", "zenith65430", "binary5260"
};

// Calculate Manhattan distance
ll dist(ll x1, ll y1, ll x2, ll y2) { return abs(x1 - x2) + abs(y1 - y2); }

// Find nearest charger for a given location(r,c) by simply looping through all chargers
pll nearestCharger(ll r, ll c) {
  ll minDist = LLONG_MAX;
  pll best = {-1, -1};
  rep(i, 0, P - 1) {
    ll d = dist(r, c, chargeLoc[i].first, chargeLoc[i].second);
    if (d < minDist) {
      minDist = d;
      best = chargeLoc[i];
    }
  }
  return best;
}

string dir = "UDLR";
vll dx = {0, 0, -1, 1};
vll dy = {1, -1, 0, 0};
// Row is x coord, Column is y coord
int moveDir(ll r1, ll c1, ll r2, ll c2) {
  if (r1 > r2) {
    return 2;
  } else if (r1 < r2) {
    return 3;
  } else if (c1 > c2) {
    return 1;
  } else if (c1 < c2) {
    return 0;
  } else {
    return -1;
  }
  return -1;
}

vll charged;
int allCharged = 0;

// The function droneMove which either moves the drone towards the nearest
// charger or charges it if it is already at a charger

void droneMove(ll left, ll i) {
  // Check if charging station
  if (charge[droneLoc[i].first][droneLoc[i].second] != -1) {
    cout << "CHARGE" << "\n";
    battery[i] = min(Bmax, battery[i] + R);
    if (battery[i] == Bmax && charged[i] == 0) {
      charged[i] = 1;
      allCharged++;
    }
    return;
  }

  // Check if low_battery
  pll nearest = nearestCharger(droneLoc[i].first, droneLoc[i].second);
  ll dista = dist(nearest.first, nearest.second, droneLoc[i].first,
                  droneLoc[i].second);
  // Go to charger
  if (droneLoc[i] != nearest) {
    ll dirn = moveDir(droneLoc[i].first, droneLoc[i].second, nearest.first,
                      nearest.second);
    cout << "MOVE " << dir[dirn] << "\n";
    droneLoc[i].first += dx[dirn];
    droneLoc[i].second += dy[dirn];
    //assert(battery[i] > 1);
    battery[i]--;
    return;
  } else {
    cout << "CHARGE" << "\n";
    battery[i] = min(Bmax, battery[i] + R);
    return;
  }
}

vector<vpll> dronePath;
vll droneIndex;
queue<ll> orderQueue;
vpll droneCharge;
vvll distCharge;
vvll pathCharge_next_node; // Stores the next charging station in the shortest path

/* The function chargingGraph which precomputes the shortest paths between all
pairs of charging stations using Floyd-Warshall algorithm It also populates the
distCharge and pathCharge_next_node matrices which are defined as follow: 1)
distCharge[i][j] : The shortest distance between charging station i and j 2)
pathCharge_next_node[i][j] : The next charging station to visit after i in order
to reach j in the shortest path
*/

void chargingGraph() {
  const ll INF = 1e18; // Use a large number for infinity
  distCharge.assign(P, vll(P, INF));
  pathCharge_next_node.assign(P, vll(P, -1)); // -1 indicates no path

  // 1. Initialize the graph with direct distances (adjacency matrix)
  rep(i, 0, P - 1) {
    rep(j, 0, P - 1) {
      if (i == j) {
        distCharge[i][j] = 0;
        pathCharge_next_node[i][j] = i;
      } else {
        ll direct_dist = dist(chargeLoc[i].first, chargeLoc[i].second,
                              chargeLoc[j].first, chargeLoc[j].second);
        if (direct_dist < Bmax - 20) {
          distCharge[i][j] = direct_dist;
          pathCharge_next_node[i][j] = j;
        }
      }
    }
  }

  // 2. Floyd-Warshall Algorithm
  // For every pair (i, j), check if going through an intermediate station 'k'
  // is shorter.
  rep(k, 0, P - 1) {
    rep(i, 0, P - 1) {
      rep(j, 0, P - 1) {
        // Check for potential overflow before adding
        if (distCharge[i][k] != INF && distCharge[k][j] != INF &&
            distCharge[i][k] + distCharge[k][j] < distCharge[i][j]) {

          // Update the distance to the shorter path
          distCharge[i][j] = distCharge[i][k] + distCharge[k][j];

          // Update the path: to get from i to j, first head towards k
          pathCharge_next_node[i][j] = pathCharge_next_node[i][k];
        }
      }
    }
  }
}

// Reconstructs the shortest path from a start to an end charging station.
vll get_charger_path(int start_node, int end_node) {
  if (pathCharge_next_node[start_node][end_node] == -1) {
    //assert(0);
    return {};
  }

  vll path;
  int current = start_node;
  while (current != end_node) {
    path.push_back(current);
    current = pathCharge_next_node[current][end_node];
  }
  path.push_back(end_node);

  return path;
}

vvll distDepotToProf;
vector<vvll> pathDepotToProf;

void DCPpreProcess() {
  // iterate over all pairs of prof and depot
  distDepotToProf.assign(M, vll(C, 1e16));
  pathDepotToProf.assign(M, vvll(C));
  rep(d, 0, M - 1) {
    rep(c, 0, C - 1) {
      vll profPossible;
      vll depoPossible;
      // Iterate over all Charging station
      rep(p, 0, P - 1) {
        ll dist_c = dist(profLoc[c].first, profLoc[c].second,
                         chargeLoc[p].first, chargeLoc[p].second);
        if (dist_c < Bmax/2 - 10) {
          profPossible.push_back(p);
        }
        ll dist_d = dist(depotLoc[d].first, depotLoc[d].second,
                         chargeLoc[p].first, chargeLoc[p].second);
        if (dist_d < Bmax/2 - 10) {
          depoPossible.push_back(p);
        }
      }
      int n = profPossible.size();
      int m = depoPossible.size();
      rep(i, 0, n - 1) {
        rep(j, 0, m - 1) {
          ll depoToCharge = dist(depotLoc[d].first, depotLoc[d].second,
                                 chargeLoc[depoPossible[j]].first,
                                 chargeLoc[depoPossible[j]].second);
          ll profToCharge = dist(profLoc[c].first, profLoc[c].second,
                                 chargeLoc[profPossible[i]].first,
                                 chargeLoc[profPossible[i]].second);
          if (ckmin(distDepotToProf[d][c],
                    distCharge[depoPossible[j]][profPossible[i]] +
                        depoToCharge + profToCharge)) {
            pathDepotToProf[d][c] = {depoPossible[j], profPossible[i]};
          }
        }
      }
    }
  }
}

void droneMove2(ll i, ll cur) {
jump:
  if (package[i] == -1) {
    cout << "CHARGE\n";
    return;
  }
  if (droneIndex[i] == dronePath[i].size() - 1 &&
      droneLoc[i] == dronePath[i].back()) {
    if (!orderQueue.empty()) {
      ll idx = orderQueue.front();
      orderQueue.pop();
      droneIndex[i] = 0;
      package[i] = idx;

      pll depotCharger = depotNearCharge[orders[idx][1]];
      dronePath[i].clear();
      ll curChargerID = charge[droneLoc[i].first][droneLoc[i].second];
      ll depotChargerID = charge[depotCharger.first][depotCharger.second];

      vll newPa = get_charger_path(curChargerID, depotChargerID);
      rep(j, 1, (int)newPa.size() - 1) {
        dronePath[i].push_back(chargeLoc[newPa[j]]);
      }
      ll dep = orders[idx][1];
      ll profe = orders[idx][2];
      dronePath[i].push_back(depotLoc[dep]);
      vll pathe = pathDepotToProf[dep][profe];
      vll chargerPath = get_charger_path(pathe[0], pathe[1]);
      for (auto ele : chargerPath) {
        dronePath[i].push_back(chargeLoc[ele]);
      }
      dronePath[i].push_back(profLoc[profe]);
      dronePath[i].push_back(profNearCharge[profe]);
    } else {
      package[i] = -1;
      cout << "CHARGE" << "\n";
      return;
    }
  }
  if (charge[droneLoc[i].first][droneLoc[i].second] != -1) {
    //assert(droneIndex[i] < (int)dronePath[i].size() && droneIndex[i] >= 0);
    if (battery[i] != Bmax) {
      battery[i] = min(Bmax, battery[i] + R);
      cout << "CHARGE" << "\n";
      return;
    } else {
      //assert(droneIndex[i] < (int)dronePath[i].size() && droneIndex[i] >= 0);
      pll target = dronePath[i][droneIndex[i]];
      if (target == droneLoc[i]) {
        droneIndex[i]++;
        //assert(droneIndex[i] < (int)dronePath[i].size() && droneIndex[i] >= 0);
        target = dronePath[i][droneIndex[i]];
        goto jump;
      }
      if (droneIndex[i] == (int)dronePath[i].size() - 2) {
        if (orders[package[i]][3] - cur > dist(target.first, target.second,
                                               droneLoc[i].first,
                                               droneLoc[i].second)) {
          cout << "CHARGE" << "\n";
          return;
        } else {
          ll dirn = moveDir(droneLoc[i].first, droneLoc[i].second, target.first,
                            target.second);
          cout << "MOVE " << dir[dirn] << "\n";
          droneLoc[i].first += dx[dirn];
          droneLoc[i].second += dy[dirn];
          //assert(battery[i] > 1);
          battery[i]--;
          return;
        }
      } else {
        ll dirn = moveDir(droneLoc[i].first, droneLoc[i].second, target.first,
                          target.second);
        cout << "MOVE " << dir[dirn] << "\n";
        droneLoc[i].first += dx[dirn];
        droneLoc[i].second += dy[dirn];
        //assert(battery[i] > 1);
        battery[i]--;
        return;
      }
    }
  } else {
    //assert(droneIndex[i] < (int)dronePath[i].size() && droneIndex[i] >= 0);
    pll target = dronePath[i][droneIndex[i]];
    if (droneLoc[i] != target) {
      ll dirn = moveDir(droneLoc[i].first, droneLoc[i].second, target.first,
                        target.second);
      cout << "MOVE " << dir[dirn] << "\n";
      droneLoc[i].first += dx[dirn];
      droneLoc[i].second += dy[dirn];
      //assert(droneLoc[i].first >= 0);
      //assert(battery[i] > 1);
      battery[i]--;
      return;
    } else {
      if (droneIndex[i] == (int)dronePath[i].size() - 2) {
        //assert(droneLoc[i] == profLoc[orders[package[i]][2]]);
        //assert(cur <= orders[package[i]][4]);
        // Reached destination
        if (package[i] != -1) {
          // Deliver package
          cout << "DROPOFF " << orders[package[i]][0] << "\n";
          //assert(battery[i] > 5);
          battery[i] -= 5;
          droneIndex[i]++;
          return;
        }
      } else {
        // Reached depot
        //assert(droneLoc[i] == depotLoc[orders[package[i]][1]]);
        ll idx = package[i];
        cout << "PICKUP " << orders[idx][0] << " " << passwords[orders[idx][0]]
             << "\n";
        //assert(battery[i] > 5);
        battery[i] -= 5;
        droneIndex[i]++;
        return;
      }
    }
  }
}

// Main turn function that runs on each turn
void turn(ll cur_turn) {
  if (allCharged != D && allCharged != -1) {
    rep(i, 0, D - 1) {
      //assert(battery[i] > 0);
      if (charged[i] == 1) {
        charged[i] = -1;
        ll idx = orderQueue.front();
        orderQueue.pop();
        droneIndex[i] = 0;
        package[i] = idx;
        pll depotCharger = depotNearCharge[orders[idx][1]];
        dronePath[i].clear();
        ll curChargerID = charge[droneLoc[i].first][droneLoc[i].second];
        ll depotChargerID = charge[depotCharger.first][depotCharger.second];

        vll newPa = get_charger_path(curChargerID, depotChargerID);
        rep(j, 1, (int)newPa.size() - 1) {
          dronePath[i].push_back(chargeLoc[newPa[j]]);
        }
        ll dep = orders[idx][1];
        ll profe = orders[idx][2];
        dronePath[i].push_back(depotLoc[dep]);
        vll pathe = pathDepotToProf[dep][profe];
        vll chargerPath = get_charger_path(pathe[0], pathe[1]);
        for (auto ele : chargerPath) {
          dronePath[i].push_back(chargeLoc[ele]);
        }
        dronePath[i].push_back(profLoc[profe]);
        dronePath[i].push_back(profNearCharge[profe]);
        droneMove2(i, cur_turn);
      } else if (charged[i] == -1) {
        droneMove2(i, cur_turn);
      } else {
        droneMove(cur_turn, i);
      }
    }
    return;
  }
  if (allCharged == D) {
    allCharged = -1;
    rep(i, 0, D - 1) {
      if (charged[i] == -1)
        continue;
      ll idx = orderQueue.front();
      orderQueue.pop();
      droneIndex[i] = 0;
      package[i] = idx;
      pll depotCharger = depotNearCharge[orders[idx][1]];
      dronePath[i].clear();
      ll curChargerID = charge[droneLoc[i].first][droneLoc[i].second];
      ll depotChargerID = charge[depotCharger.first][depotCharger.second];

      vll newPa = get_charger_path(curChargerID, depotChargerID);
      rep(j, 1, (int)newPa.size() - 1) {
        dronePath[i].push_back(chargeLoc[newPa[j]]);
      }
      ll dep = orders[idx][1];
      ll profe = orders[idx][2];
      dronePath[i].push_back(depotLoc[dep]);
      vll pathe = pathDepotToProf[dep][profe];
      vll chargerPath = get_charger_path(pathe[0], pathe[1]);
      for (auto ele : chargerPath) {
        dronePath[i].push_back(chargeLoc[ele]);
      }
      dronePath[i].push_back(profLoc[profe]);
      dronePath[i].push_back(profNearCharge[profe]);
    }
  }
  rep(i, 0, D - 1) {
    //assert(battery[i] > 0);
    droneMove2(i, cur_turn);
  }
}
void profAndDepoToCharge() {
  // Iterate over all charging for all prof
  rep(i, 0, C - 1) {
    pll nearest = nearestCharger(profLoc[i].first, profLoc[i].second);
    profNearCharge[i] = nearest;
  }
  // Iterate over all depots
  rep(i, 0, M - 1) {
    pll nearest = nearestCharger(depotLoc[i].first, depotLoc[i].second);
    depotNearCharge[i] = nearest;
  }
}

vll sortedOrders;
vll whiteSortedOrders;
vll trimmedSorted;

// Preprocessing the list of orders to process
void orderPreProcess() {
  sortedOrders.resize(N);
  iota(sortedOrders.begin(), sortedOrders.end(), 0);
  sort(sortedOrders.begin(), sortedOrders.end(), [](int a, int b) {
    ll val1 = (orders[a][4] - orders[a][3]) * orders[a][5];
    ll val2 = (orders[b][4] - orders[b][3]) * orders[b][5];
    return val1 > val2;
  });
  rep(i, 0, N - 1) {
    ll c = orders[sortedOrders[i]][2];
    ll d = orders[sortedOrders[i]][1];
    bool pro = false;
    bool dep = false;
    rep(p, 0, P - 1) {
      ll dist_c = dist(profLoc[c].first, profLoc[c].second, chargeLoc[p].first,
                       chargeLoc[p].second);
      if (dist_c < Bmax/2 - 10) {
        pro = true;
      }
      ll dist_d = dist(depotLoc[d].first, depotLoc[d].second,
                       chargeLoc[p].first, chargeLoc[p].second);
      if (dist_d < Bmax/2 - 10) {
        dep = true;
      }
    }
    if (pro == true && dep == true) {
      whiteSortedOrders.push_back(sortedOrders[i]);
    }
  }
  ll cnt = N;
  for (int i = 0; i < cnt && i < whiteSortedOrders.size(); i++) {
    trimmedSorted.push_back(whiteSortedOrders[i]);
  }
  sort(all(trimmedSorted),
       [](int a, int b) { return orders[a][3] < orders[b][3]; });
  for (auto ele : trimmedSorted) {
    orderQueue.push(ele);
  }
}

int main() {
  freopen("input.txt", "r", stdin);
  freopen("output.txt", "w", stdout);
  input();
  profAndDepoToCharge();
  charged.assign(D, 0);
  chargingGraph();
  orderPreProcess();
  DCPpreProcess();
  dronePath.resize(D);
  droneIndex.assign(D, -1);
  droneCharge.assign(D, {-1, -1});
  rep(i, 0, T - 1) {
    turn(i);
    cout << "\n";
  }
  return 0;
}