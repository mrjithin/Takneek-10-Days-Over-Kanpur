#include <bits/stdc++.h>
using namespace std;

//#define cerr if (0) cerr

struct Drone {
  int x;
  int y;
  int b;
  int i = -1;
  int d = -1;
};

struct Order {
  int t;
  int d;
  int p;
  int s;
  int e;
  int v;
};

const array<string, 48> kPassword = {
    "horizon92",    "zenith617",     "yarrow32",    "obelisk7053",
    "dagger27",     "lattice23",     "nebula54793", "obelisk4985",
    "zephyr54",     "zephyr43239",   "amber97707",  "quantum69715",
    "zenith4286",   "anchor92738",   "dagger170",   "cipher8009",
    "raptor3140",   "willow303",     "linchpin476", "sentinel415",
    "horizon17668", "paradox13",     "yonder71206", "monolith884",
    "packet39",     "cipher97990",   "cursor3726",  "yarrow54",
    "infosec21285", "jaguar512",     "infosec7302", "debugger4764",
    "beacon490",    "raven5288",     "delta6979",   "anchor459",
    "whisper68",    "sentinel56995", "raven65",     "nebula70",
    "canvas33",     "xenon556",      "whisper4791", "lattice80662",
    "binary71929",  "granite81942",  "zenith65430", "binary5260"};

int W, D, C, M, P, K, Tmax, Bmax, R, N;
vector<Drone> drones;
vector<pair<int, int>> prof;
vector<pair<int, int>> depot;
vector<vector<int>> inv;
vector<pair<int, int>> c_station;
vector<Order> orders;
vector<vector<int>> grid;

double Srev = 0, Pfail = 0, Plost = 0, Seff;

bool input() {
  cin >> W >> D >> C >> M >> P >> K >> Tmax >> Bmax >> R;
  N = 10000;

  grid = vector<vector<int>>(W, vector<int>(W, -1));

  drones.resize(D);
  for (auto &[x, y, d, _1, _2] : drones) {
    cin >> x >> y >> d;
  }
  prof.resize(C);
  for (auto &[x, y] : prof) {
    cin >> x >> y;
    grid[x][y] = -2;
  }
  depot.resize(M);
  for (int i = 0; i < M; i++) {
    auto &[x, y] = depot[i];
    cin >> x >> y;
    grid[x][y] = i;
  }
  inv.resize(M);
  for (auto &v : inv) {
    v.resize(K);
    for (int &x : v)
      cin >> x;
  }
  c_station.resize(P);
  for (auto &[x, y] : c_station) {
    cin >> x >> y;
    grid[x][y] = -3;
  }
  orders.resize(N);
  for (auto &[t, d, p, s, e, v] : orders) {
    cin >> t >> d >> p >> s >> e >> v;
    Pfail += 1.8 * v;
  }
  return true;
}

int cnt_fail = 0;

int main() {
  freopen("input.txt", "r", stdin);
  freopen("final_score.txt", "w", stdout);

  input();

  bool executed = false;
  auto move = [&](const int tt, const int i) {
    char c;
    cin >> c;
    int dx = 0, dy = 0;
    if (c == 'U') {
      dx = 0, dy = 1;
    } else if (c == 'D') {
      dx = 0, dy = -1;
    } else if (c == 'L') {
      dx = -1, dy = 0;
    } else if (c == 'R') {
      dx = 1, dy = 0;
    } else {
      cerr << "Invalid movement command at time, drone: " << tt << ' ' << i
           << ". Performing STAY.\n";
      return;
    }
    auto &[x, y, b, item, _] = drones[i];
    const int x_ = x + dx, y_ = y + dy;
    executed = (0 <= x_ and x_ < W and 0 <= y_ and y_ < W);
    if (executed) {
      x = x_, y = y_;
      drones[i].b--;
    } else {
      cerr << "Drone tried to move outside grid from (x, y) to (x', y'): " << x
           << ' ' << y << ' ' << x_ << ' ' << y_ << '\n';
    }
  };

  auto pickup = [&](const int tt, const int i) {
    int item;
    cin >> item;
    string pwd;
    cin >> pwd;
    if (pwd != kPassword[item]) {
      cerr << "Incorrect Password Entered! Time, Drone = " << tt << ' ' << i
           << '\n';
    }
    const int x = drones[i].x;
    const int y = drones[i].y;
    const int dep = grid[x][y];
    if (drones[i].i != -1) {
      cerr << "Drone " << i << " already has item " << drones[i].i
           << " picked. Cannot pick item " << item << ". Performing STAY.\n";
    } else if (dep < 0) {
      cerr << "Drone is not at the location of a depot. Location: " << x << ' '
           << y << ". Time, Drone: " << tt << ' ' << i
           << ". Performing STAY.\n";
    } else if (inv[dep][item] == 0) {
      cnt_fail++;
      cerr << "Depot doesn't have any more items of type " << i
           << ". Time, Drone: " << tt << ' ' << i << ". Performing STAY.\n";
    } else {
      executed = true;
      drones[i].i = item;
      drones[i].d = dep;
      inv[dep][drones[i].i]--;
    }
    drones[i].b -= 5 * executed;
  };

  auto dropoff = [&](const int tt, const int i) {
    int item;
    cin >> item;
    if (drones[i].i == -1) {
      cerr << "Attempt at dropping item when empty. time, drone: " << tt << ' '
           << i << ". Performing STAY.\n";
      return;
    }
    const int x = drones[i].x;
    const int y = drones[i].y;
    if (grid[x][y] != -2) {
      cerr << "Attempt at dropping item at non-prof position. time, drone: "
           << tt << ' ' << i << "Performing STAY.\n";
      return;
    }
    bool found = false;
    for (int oo = 0; oo < orders.size(); oo++) {
      if (orders[oo].t != drones[i].i)
        continue;
      if (orders[oo].d != drones[i].d)
        continue;
      if (prof[orders[oo].p].first != drones[i].x)
        continue;
      if (prof[orders[oo].p].second != drones[i].y)
        continue;
      if (tt < orders[oo].s)
        continue;
      if (orders[oo].e < tt)
        continue;
      Pfail -= 1.8 * orders[oo].v;
      Srev += orders[oo].v + (orders[oo].e - tt) * orders[oo].v * 0.003;
      found = true;
      orders.erase(orders.begin() + oo);
      break;
    }
    if (not found) {
      cerr << "No valid order found with (d, item, dep, x, y): " << i << ' '
           << item << ' ' << drones[i].d << ' ' << x << ' ' << y
           << ". Performing STAY.";
    } else {
      executed = true;
      drones[i].b -= 5;
      drones[i].i = -1;
      drones[i].d = -1;
    }
  };

  freopen("output.txt", "r", stdin);
  for (int tt = 0; tt < Tmax; tt++) {

    for (int i = 0; i < D; i++) {
      string s;
      cin >> s;
      executed = false;
      if (s == "MOVE") {
        move(tt, i);
      } else if (s == "PICKUP") {
        pickup(tt, i);
      } else if (s == "DROPOFF") {
        dropoff(tt, i);
      } else if (s == "CHARGE") {
        drones[i].b += R;
        drones[i].b = min(drones[i].b, Bmax);
        executed = true;
      }
      if (not executed) {
        if (s != "STAY") {
          cerr << "Invalid command at time " << tt << " and drone " << i
               << ". Performing the STAY operation.\n";
        }
        drones[i].b--;
      }
      if (drones[i].b < 0) {
        cerr << "DRONE " << i << " DIED!!!\n";
        Plost += 1e8;
      }
    }
  }

  for (int i = 0; i < D; i++) {
    Seff += 0.1 * drones[i].b;
  }

  cout << "Srev: " << Srev << '\n';
  cout << "Pfail: " << Pfail << '\n';
  cout << "Plost: " << Plost << '\n';
  cout << "Seff: " << Seff << '\n';
  cout << "Final Score: " << 1e6 + Srev - Pfail - Plost + Seff;
  cout << "Failed orders: " << cnt_fail << '\n';
}
