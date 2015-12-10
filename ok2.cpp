#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <random>

using namespace std;
using namespace std::chrono;

class Customer {
public:
    int id;
    int x;
    int y;
    int demand;
    int ready_time;
    int due_date;
    int service_duration;
    int atPosition;
};

bool operator==(const Customer &me, const Customer &another) {
    return me.id == another.id;
}

class Saving {
public:
    int i, j;
    double saving;
};

class Route {
public:
    vector<Customer> route;
    /*int idL, idR;
    double len;
    int time;
    int cap;*/
};

//Zmienne globalne
int vehiclesNumber;
int vehiclesCapacity;
double routesLength = 0;

random_device rd;
mt19937 rng(rd());



vector<Customer> customersVector;

//Funkcja wypisujaca kontrolnie vector
void print_customersVector(vector<Customer> &v) {
    vector<Customer>::iterator it;
    for (it = v.begin(); it != v.end(); ++it) {
        cout << (*it).id << " " << (*it).x << " " << (*it).y << " " << (*it).demand << " " << (*it).ready_time << " " <<
        (*it).due_date << " " << (*it).service_duration << endl;
    }
}

//Funkcja wypisujaca kontrolnie pojedynczego klienta
void print_Customer(Customer v) {
    cout << v.id << " " << v.x << " " << v.y << " " << v.demand << " " << v.ready_time << " " << v.due_date << " " <<
    endl;
}


//Funkcja wczytujaca dane z pliku w formacie solomona
void data_input(char *filename) {

    string truncLine;
    fstream file;
    file.open(filename, fstream::in);

    Customer temp;

    //Omijanie wstepnych linii bez danych
    for (int i = 0; i < 4; i++)
        getline(file, truncLine);

    file >> vehiclesNumber >> vehiclesCapacity;

    //Wlaczenie wypisywania kontrolnego liczby ciezarowek i ich ladownosci
    //printf("%d %d\n", vehiclesNumber,vehiclesCapacity);

    //Omijanie kolejnych linii bez danych
    for (int i = 0; i < 4; i++)
        getline(file, truncLine);

    while (!file.eof()) {
        file >> temp.id >> temp.x >> temp.y >> temp.demand >> temp.ready_time >> temp.due_date >> temp.service_duration;
        temp.atPosition = temp.id;
        customersVector.push_back(temp);
    }

    //Wyrzucam ostatni element z vectora, bo ze wzgledu na pusta ostatnia linie pliku powtarza sie ostatni customer
    customersVector.pop_back();


    file.close();
}

//Funkcja wczytywania okreslonej liczby danych
void data_input_n(char *filename, char *data_quantity) {
    string truncLine;
    fstream file;
    file.open(filename, fstream::in);
    int n;

    Customer temp;

    //Omijanie wstepnych linii bez danych
    for (int i = 0; i < 4; i++)
        getline(file, truncLine);

    file >> vehiclesNumber >> vehiclesCapacity;

    //Wlaczenie wypisywania kontrolnego liczby ciezarowek i ich ladownosci
    //printf("%d %d\n", vehiclesNumber,vehiclesCapacity);

    //Omijanie kolejnych linii bez danych
    for (int i = 0; i < 4; i++)
        getline(file, truncLine);

    n = atoi(data_quantity);

    //Wczytywanie danych magazynu i n klientow

    for (int i = 0; i <= n && !file.eof(); i++) {
        file >> temp.id >> temp.x >> temp.y >> temp.demand >> temp.ready_time >> temp.due_date >> temp.service_duration;
        temp.atPosition = temp.id;
        customersVector.push_back(temp);
    }


    //Wyrzucanie ostatniego elementu z vectora, gdy dotrzemy do konca pliku (ostatni element jest taki sam jak przedostatni)
    if (file.eof()) {
        customersVector.pop_back();
    }

    file.close();
}


//Funkcja obliczajaca dlugosc trasy miedzy dwoma punktami
double distance1(Customer point1, Customer point2) {
    return sqrt(((point1.x - point2.x) * (point1.x - point2.x)) + ((point1.y - point2.y) * (point1.y - point2.y)));

}

high_resolution_clock::time_point startClock() {
    return high_resolution_clock::now();
}

long double endClock(high_resolution_clock::time_point timePoint) {
    auto timePoint2 = high_resolution_clock::now();
    nanoseconds span = duration_cast<nanoseconds>(timePoint2 - timePoint);
    return span.count();
}

//Wypisanie pojedynczej drogi
void printRoute(Route route) {
    for (unsigned long i = 1; i < route.route.size() - 1; ++i) {
        printf("%i ", route.route.at(i).id);
    }
    printf("\n");
}

//Wypisanie wszystkich drog
void printRoutes(vector<Route> routes) {
    if (routes.empty())
        cout << "-1";
    for (auto r:routes) {
        printRoute(r);
    }
}



//Funkcja sprawdzajaca poprawnosc trasy, gdy jest poprawna zwraca jej dlugosc
double isConnectionFeasible(Route route) {
    double distanceSum = 0;
    int capacitySum = 0;

    unsigned long size = route.route.size();

    for (unsigned long i = 1; i < size; ++i) {
        Customer it1 = route.route.at(i - 1);
        Customer it2 = route.route.at(i);
        if (distanceSum + distance1(it1, it2) <= it2.due_date) {
            distanceSum += distance1(it1, it2);
            if (distanceSum < it2.ready_time) {
                distanceSum = it2.ready_time;
            }
            distanceSum += it2.service_duration;
        }
        else
            return -1;
        if (capacitySum + it2.demand <= vehiclesCapacity)
            capacitySum += it2.demand;
        else
            return -1;

    }
    routesLength += distanceSum;
    return distanceSum;

}

//Tworzenie poczatkowego naiwnego rozwiazania
vector<Route> createNaiveRoutes(){
    bool used[customersVector.size()];


    vector<Route> routes;
    Route tempRoute;

    int routeNumber=0;
    int position=0;

    //Tworzenie drog magazyn - klient - magazyn
    for(int i=1;i<customersVector.size();i++){
        used[i]=false;
        tempRoute.route.push_back(customersVector.at(0));
        tempRoute.route.push_back(customersVector.at(i));
        tempRoute.route.push_back(customersVector.at(0));
        routes.push_back(tempRoute);
        tempRoute.route.clear();
    }

    //printRoutes(routes);

    bool removed[routes.size()];
    for(int i=0;i<routes.size();i++)
        removed[i]=false;

    //Naiwne laczenie drog
    for(int j=0;j<routes.size();j++){
        if(!used[j+1]){
            for(int i=1;i<customersVector.size();i++){
                tempRoute=routes.at(j);
                tempRoute.route.insert(tempRoute.route.end()-1,customersVector.at(i));
                if(!used[i] && i!=j+1 && isConnectionFeasible(tempRoute)!=-1){
                    routes.at(j)=tempRoute;
                    used[i]=true;
                    used[j+1]=true;
                    removed[i-1]=true;
                }
            }
        }
        }

    //Usuwanie drog zawierajacych uzyte wierzcholki
    for(int j=routes.size()-1;j>=0;j--) {
        //printRoute(routes.at(j));
       if (removed[j]) {
            routes.erase(routes.begin() + j);
           }
    }

    //printRoutes(routes);


return routes;

}



vector<Route> performMove1(vector<Route> routes){


	uniform_int_distribution<int> irand(0,routes.size()-1);
	int r = irand(rng);
	
	uniform_int_distribution<int> krand(1,routes.at(r).route.size()-2);
	int k=krand(rng);
    int l=krand(rng);
	swap(routes.at(r).route.at(k), routes.at(r).route.at(l));
	return routes;
}

vector<Route> performMove2(vector<Route> routes){
	if(routes.size()==1) return routes;
    uniform_int_distribution<int> irand(0,routes.size()-1);

    int i = irand(rng);

    uniform_int_distribution<int> jrand(0,routes.size()-2);

    //cout<<"TUTAJ"<<endl;
    int j = jrand(rng);
	if(i>=j) ++j;

    //cout<<i<<" "<<j<<endl;

    uniform_int_distribution<int> krand(1,routes.at(i).route.size()-2);
    int k=krand(rng);


    uniform_int_distribution<int> lrand(1,routes.at(j).route.size()-2);
    int l=lrand(rng);

    routes.at(i).route.insert(routes.at(i).route.begin()+k,routes.at(j).route.at(l));
    routes.at(j).route.erase(routes.at(j).route.begin()+l);

    if(routes.at(j).route.front().id==0 && routes.at(j).route.back().id==0 && routes.at(j).route.size()==2)
        routes.erase(routes.begin()+j);

    return routes;
}

vector<Route> performMove3(vector<Route> routes){
	if(routes.size()==1) return routes;	

	int S=1000000;
	int At=-1;
	int i=0;
	for(auto R: routes){
		int s = R.route.size();
		if(s<S){ S=s; At=i; }
		++i;
	}

	uniform_int_distribution<int> irand(0,routes.size()-2);
	int p = irand(rng);
	if(p>=At){
		++p;
	}
	
	uniform_int_distribution<int> krand(1,routes.at(At).route.size()-2);
    int l=krand(rng);


    uniform_int_distribution<int> lrand(1,routes.at(p).route.size()-2);
    int k=lrand(rng);
	
    routes.at(p).route.insert(routes.at(p).route.begin()+k,routes.at(At).route.at(l));
    routes.at(At).route.erase(routes.at(At).route.begin()+l);
	
    if(routes.at(At).route.front().id==0 && routes.at(At).route.back().id==0 && routes.at(At).route.size()==2)
        routes.erase(routes.begin()+At);
    return routes;
}

vector<Route> performMove4(vector<Route> routes){
    return routes;
}

//Stworzenie nowych drog przez wykonanie jednej ze zmian
vector<Route> performMoves(vector<Route> routes){
	uniform_int_distribution<int> irand(1, 3); //1, 4
	int r = irand(rng);
	
	switch(r){
		case 1:
			return performMove1(routes);
		case 2:
			return performMove2(routes);
		case 3:
			return performMove3(routes);
		case 4:
			return performMove4(routes);
		default:
			throw -1;
	}
}

double routesDistance(vector<Route> routes){

    double distanceSum=0;
    double distance;

    for(auto r:routes){
        distance=isConnectionFeasible(r);
        if(distance==-1){
            return -1;
        }
        distanceSum+=distance;
    }

    return distanceSum;

}


bool decision(double currentDistance, double tempDistance, double temperature){

    bool doItOrNot=false;
    double k=25;

    double prob;
    double random;

    random=(rand()%101)/100.00;
    prob=1.0/(exp((((tempDistance-currentDistance)*25))/(currentDistance*temperature)));

    //cout<<random<<endl;
    //cout<<prob<<endl<<endl;

    if(prob>random){
        doItOrNot=true;
    }

    return doItOrNot; // just do it!
}


vector<Route> performAnnealing(vector<Route> routes, double startTemperature, double endTemperature, double coolingFactor, int innerIterations, vector<Route> bestRoutes){

	srand(time(0));
    double temperature=startTemperature;
    vector<Route> tempRoutes;

    double currentDistance=routesDistance(routes);
    double bestDistance=currentDistance;
    double tempDistance;

    while(temperature>endTemperature){



        for(int h=0;h<innerIterations;h++){



            tempRoutes=performMoves(routes);


            tempDistance=routesDistance(tempRoutes);

            if(tempDistance!=-1){

                if(currentDistance>tempDistance){
                    routes=tempRoutes;
                    currentDistance=tempDistance;
                    if(currentDistance<bestDistance){
                        bestRoutes=routes;
                        bestDistance=currentDistance;
                    }
                }
                else if(decision(currentDistance,tempDistance, temperature)){
                    routes=tempRoutes;
                    currentDistance=tempDistance;
                }

            }

        }

        temperature=temperature*coolingFactor;
    }


    return bestRoutes;
}

void save(char *name, vector<Route> routes) {
    ofstream o;
    o.open(name, ofstream::out | ofstream::trunc);
    if (routes.empty())
        o << "-1\n";
    else {
        double sum = 0;
        for (auto i: routes) {
            sum += isConnectionFeasible(i);
        }
        o<<routes.size()<<" "<< fixed << setprecision(5)<<sum<<endl;

        if (routes.empty())
            cout << "-1";
        for (auto route:routes) {
            vector<Customer> r = route.route;
            for (unsigned long i = 1; i < r.size() - 1; ++i) {
                o<<r.at(i).id<<" ";
            }
            o<<endl;
        }
    }
    o.close();
}

int main(int argc, char *argv[]) {

    srand (time(NULL));
    //random_device rd;

    //Domyslna nazwa pliku wynikowego
    string defaultOutFileName="wynik.txt";
    char *cdefaultOutFileName = new char[defaultOutFileName.length() + 1];
    strcpy(cdefaultOutFileName, defaultOutFileName.c_str());

    char *ile = nullptr, *fileName = nullptr, *outFileName = cdefaultOutFileName;

    if (argc < 2) {
        printf("Niewlasciwa liczba parametrow\n");
        return 1;
    }
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-n") {
            ile = argv[++i];
            continue;
        }
        if (arg == "-o") {
            outFileName = argv[++i];
            continue;
        }
        else
            fileName = argv[i];
    }

    if (ile == nullptr)
        data_input(fileName);
    else
        data_input_n(fileName, ile);



    vector<Route> routes;
    vector<Route> bestRoutes;



    routes=createNaiveRoutes();

    bestRoutes=routes;

    /*printRoutes(routes);


    cout<<endl<<endl;*/


    routes=performAnnealing(routes, 0.8, 0.01, 0.995, 60,bestRoutes);



    /*for(int i=0;i<10;i++){

        routes=performMove2(routes);
        printRoutes(routes);
        cout<<endl<<endl;
    }
     */


    //Wlaczanie kontrolnego wypisywania vectora
    //print_customersVector(customersVector);


    try {
        for (auto i: routes){
            if (isConnectionFeasible(i) == -1) {
                throw -1;
            }
        }
    } catch (int e) {
        printf("-1\n");
        routes.clear();
        if (outFileName != nullptr)
            save(outFileName, routes);
        return 0;
    }

    double sum = 0;
    for (auto i: routes) {
        sum += isConnectionFeasible(i);
    }


    //Wyswietlanie wynikow dzialania algorytmu na stdout

    //printf("%lu %0.5f\n", routes.size(), sum);

    //printRoutes(routes);

    //if (outFileName != nullptr)
       save(outFileName, routes);


    return 0;
}
