#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include <fstream>

using namespace std;
using namespace std::chrono;

// ==================== INGREDIENT ====================
struct Vitamin { string name; Vitamin(string n = "") : name(n) {} };
struct Mineral { string name; Mineral(string n = "") : name(n) {} };

class Ingredient {
    string name;
    double protein, calories, fat, carb;
    Vitamin vitamin;
    Mineral mineral;
    double stock;
    double pricePerKg;
public:
    Ingredient(string n, double p = 0, double cal = 0, double f = 0, double c = 0,
               Vitamin v = Vitamin(), Mineral m = Mineral(), double s = 0, double price = 0)
        : name(n), protein(p), calories(cal), fat(f), carb(c), vitamin(v), mineral(m),
          stock(s), pricePerKg(price) {}

    string GetName() const { return name; }
    double GetStock() const { return stock; }
    double GetPrice() const { return pricePerKg; }

    // Setters with exception handling
    void SetName(const string& n) { if(n.empty()) throw runtime_error("Ingredient name cannot be empty"); name = n; }
    void SetProtein(double p) { if(p<0) throw runtime_error("Protein cannot be negative"); protein=p; }
    void SetCalories(double c) { if(c<0) throw runtime_error("Calories cannot be negative"); calories=c; }
    void SetFat(double f) { if(f<0) throw runtime_error("Fat cannot be negative"); fat=f; }
    void SetCarb(double c) { if(c<0) throw runtime_error("Carb cannot be negative"); carb=c; }
    void AddStock(double s) { if(s<=0) throw runtime_error("Stock to add must be positive"); stock+=s; }
    void ReduceStock(double s) { if(s<=0) throw runtime_error("Stock to reduce must be positive"); if(s>stock) throw runtime_error("Not enough stock"); stock-=s; }
    void SetPrice(double p) { if(p<0) throw runtime_error("Price cannot be negative"); pricePerKg=p; }
};

// ==================== FOOD ====================
enum MeasureType { Count=1, Weight, Litr };

class Food {
    string name;
    string description;
    double salePrice;
    MeasureType measureType;
    double amount;
    map<Ingredient*, double> ingredients;
public:
    Food(string n, double sp, string desc="", MeasureType mt=Count, double amt=1)
        : name(n), salePrice(sp), description(desc), measureType(mt), amount(amt) {}

    string GetName() const { return name; }
    double GetSalePrice() const { return salePrice; }
    const map<Ingredient*, double>& GetIngredients() const { return ingredients; }

    void SetName(const string& n){ if(n.empty()) throw runtime_error("Food name cannot be empty"); name=n; }
    void SetSalePrice(double p){ if(p<0) throw runtime_error("Food price cannot be negative"); salePrice=p; }

    void AddIngredient(Ingredient* ing, double qty){
        if(!ing) throw runtime_error("Ingredient cannot be null");
        if(qty<=0) throw runtime_error("Ingredient quantity must be positive");
        ingredients[ing]=qty;
    }
};

// ==================== MEAL ====================
class Meal {
    string name;
    vector<Food*> foods;
public:
    Meal(string n): name(n) {}
    string GetName() const { return name; }
    const vector<Food*>& GetFoods() const { return foods; }

    void SetName(const string& n){ if(n.empty()) throw runtime_error("Meal name cannot be empty"); name=n; }
    void AddFood(Food* f){ if(!f) throw runtime_error("Food cannot be null"); foods.push_back(f); }

    double GetTotalPrice() const { double total=0; for(auto f: foods) total+=f->GetSalePrice(); return total; }
};

// ==================== ORDER HISTORY ====================
struct OrderRecord {
    vector<Meal*> meals;
    double totalPrice;
    system_clock::time_point timestamp;
    OrderRecord(const vector<Meal*>& m,double price): meals(m), totalPrice(price), timestamp(system_clock::now()) {}
};

class History {
    vector<OrderRecord> orders;
public:
    void AddOrder(const vector<Meal*>& cart,double totalPrice){ orders.push_back(OrderRecord(cart,totalPrice)); }

    void ShowAllOrders() const {
        if(orders.empty()){ cout<<"No orders yet.\n"; return; }
        cout<<"\n===== ORDER HISTORY =====\n";
        for(size_t i=0;i<orders.size();++i){
            const auto& order=orders[i];
            auto t_c = system_clock::to_time_t(order.timestamp);
            tm tm_local{};
#ifdef _WIN32
            localtime_s(&tm_local,&t_c);
#else
            localtime_r(&t_c,&tm_local);
#endif
            cout<<"Order #"<<i+1<<" - "<<put_time(&tm_local,"%Y-%m-%d %H:%M:%S")<<"\n";
            cout<<"Total Price: "<<order.totalPrice<<" AZN\nItems:\n";
            for(auto m:order.meals) cout<<"  "<<m->GetName()<<"\n";
            cout<<"------------------------\n";
        }
    }

    void SaveToFile(const string& filename) const {
        ofstream file(filename, ios::app);
        if(!file) throw runtime_error("Cannot open file for writing");
        for(const auto& order: orders){
            auto t_c = system_clock::to_time_t(order.timestamp);
            tm tm_local{};
#ifdef _WIN32
            localtime_s(&tm_local,&t_c);
#else
            localtime_r(&t_c,&tm_local);
#endif
            file<<put_time(&tm_local,"%Y-%m-%d %H:%M:%S")<<" | "<<order.totalPrice<<" AZN | ";
            for(auto m:order.meals) file<<m->GetName()<<" ";
            file<<"\n";
        }
    }
};

// ==================== PERSON ====================
class Person{
protected:
    string firstName,lastName,email,password;
public:
    Person(string f="",string l="",string e="",string p=""):firstName(f),lastName(l),email(e),password(p){}
    virtual string GetRole() const=0;
    string GetEmail() const { return email; }
    string GetPassword() const { return password; }
};

class Admin: public Person {
public:
    Admin(string f,string l,string e,string p):Person(f,l,e,p){}
    string GetRole() const override { return "Admin"; }
};

class User: public Person {
    History history;
public:
    User(string f,string l,string e,string p):Person(f,l,e,p){}
    string GetRole() const override { return "User"; }
    void AddOrder(const vector<Meal*>& cart,double totalPrice){ history.AddOrder(cart,totalPrice); }
    void ShowHistory() const { history.ShowAllOrders(); }
    void SaveHistoryToFile(const string& filename){ history.SaveToFile(filename); }
};

// ==================== CART ====================
class Cart{
    vector<Meal*> cart;
public:
    void Add(Meal* m){ if(!m) throw runtime_error("Meal cannot be null"); cart.push_back(m); cout<<"Added to cart.\n"; }
    void Clear(){ cart.clear(); cout<<"Cart cleared.\n"; }
    double Total() const { double t=0; for(auto m:cart) t+=m->GetTotalPrice(); return t; }
    vector<Meal*> GetCart() const { return cart; }
    void Show() const { if(cart.empty()){ cout<<"Cart empty.\n"; return; } cout<<"\n===== CART =====\n"; for(size_t i=0;i<cart.size();++i) cout<<i+1<<". "<<cart[i]->GetName()<<" - "<<cart[i]->GetTotalPrice()<<" AZN\n"; cout<<"Total: "<<Total()<<" AZN\n"; }
};

// ==================== RESTAURANT ====================
class Restaurant{
    vector<Meal*> meals;
    vector<Ingredient*> ingredients;
    double budget=0;
public:
    void AddMeal(Meal* m){ if(!m) throw runtime_error("Meal null"); meals.push_back(m); }
    void AddIngredient(Ingredient* i){ if(!i) throw runtime_error("Ingredient null"); ingredients.push_back(i); }
    vector<Meal*>& GetMeals(){ return meals; }
    vector<Ingredient*>& GetIngredients(){ return ingredients; }
    void ShowMeals() const{ cout<<"\n===== MENU =====\n"; for(size_t i=0;i<meals.size();++i) cout<<i+1<<". "<<meals[i]->GetName()<<" - "<<meals[i]->GetTotalPrice()<<" AZN\n"; }
    void AddBudget(double amount){ if(amount<0) throw runtime_error("Budget cannot be negative"); budget+=amount; }
    double GetBudget() const { return budget; }
};

// ==================== AUTH SYSTEM ====================
class AuthSystem{
    Admin* admin=nullptr;
    User* user=nullptr;
    string currentRole="";
public:
    bool LoggedIn() const { return currentRole!="" ; }
    string GetRole() const { return currentRole; }
    Admin* GetAdmin() { return admin; }
    User* GetUser() { return user; }

    void Register(){
        int role; cout<<"1. Admin 2. User: "; cin>>role; cin.ignore();
        string f,l,e,p;
        cout<<"First: "; getline(cin,f);
        cout<<"Last: "; getline(cin,l);
        cout<<"Email: "; getline(cin,e);
        cout<<"Password: "; getline(cin,p);

        if(role==1) admin=new Admin(f,l,e,p);
        else if(role==2) user=new User(f,l,e,p);
        else throw runtime_error("Invalid role selected");
        cout<<"Registered!\n";
    }

    bool Login(){
        int role; cout<<"1. Admin 2. User: "; cin>>role; cin.ignore();
        string e,p;
        cout<<"Email: "; getline(cin,e);
        cout<<"Password: "; getline(cin,p);

        if(role==1 && admin && e==admin->GetEmail() && p==admin->GetPassword()){ currentRole="Admin"; return true; }
        else if(role==2 && user && e==user->GetEmail() && p==user->GetPassword()){ currentRole="User"; return true; }
        else { cout<<"Wrong credentials\n"; return false; }
    }

    void Logout(){ currentRole=""; cout<<"Logged out.\n"; }
};

// ==================== MAIN ====================
int main(){
    try{
        AuthSystem auth;
        Restaurant restaurant;
        Cart cart;

        // Sample Ingredients
        Ingredient* tomato=new Ingredient("Tomato",1,20,0,4,Vitamin(),Mineral(),10,2);
        Ingredient* cheese=new Ingredient("Cheese",25,400,30,5,Vitamin(),Mineral(),5,10);
        Ingredient* chicken=new Ingredient("Chicken",20,200,10,0,Vitamin(),Mineral(),8,7);

        // Sample Foods
        Food* f1=new Food("Tomato Salad",5,"Fresh tomato salad"); f1->AddIngredient(tomato,0.2);
        Food* f2=new Food("Cheese Pizza",15,"Extra cheese pizza"); f2->AddIngredient(cheese,0.1);
        Food* f3=new Food("Grilled Chicken",20,"Grilled chicken"); f3->AddIngredient(chicken,0.5);

        // Sample Meals
        Meal* m1=new Meal("Salad Meal"); m1->AddFood(f1);
        Meal* m2=new Meal("Pizza Meal"); m2->AddFood(f2);
        Meal* m3=new Meal("Chicken Meal"); m3->AddFood(f3);

        restaurant.AddIngredient(tomato); restaurant.AddIngredient(cheese); restaurant.AddIngredient(chicken);
        restaurant.AddMeal(m1); restaurant.AddMeal(m2); restaurant.AddMeal(m3);

        bool running=true;
        while(running){
            try{
                if(!auth.LoggedIn()){
                    cout<<"1.Login 2.Register 3.Exit: ";
                    int c; cin>>c; cin.ignore();
                    if(c==1) auth.Login();
                    else if(c==2) auth.Register();
                    else running=false;
                }
                else if(auth.GetRole()=="Admin"){
                    cout<<"1.View Meals 2.View Budget 3.Logout: ";
                    int c; cin>>c; cin.ignore();
                    if(c==1) restaurant.ShowMeals();
                    else if(c==2) cout<<"Budget: "<<restaurant.GetBudget()<<" AZN\n";
                    else auth.Logout();
                }
                else if(auth.GetRole()=="User"){
                    cout<<"1.View Meals 2.Add to Cart 3.View Cart 4.Checkout 5.Order History 6.Logout: ";
                    int c; cin>>c; cin.ignore();
                    if(c==1) restaurant.ShowMeals();
                    else if(c==2){ int choice; cout<<"Meal number: "; cin>>choice; cin.ignore(); if(choice>0 && choice<=(int)restaurant.GetMeals().size()) cart.Add(restaurant.GetMeals()[choice-1]); }
                    else if(c==3) cart.Show();
                    else if(c==4){
                        double total=cart.Total();
                        cout<<"Total "<<total<<" AZN. Confirm? y/n: ";
                        char confirm; cin>>confirm; cin.ignore();
                        if(confirm=='y'||confirm=='Y'){
                            auth.GetUser()->AddOrder(cart.GetCart(),total);
                            auth.GetUser()->SaveHistoryToFile("order_history.txt");
                            restaurant.AddBudget(total);
                            cart.Clear();
                            cout<<"Order placed!\n";
                        }
                    }
                    else if(c==5) auth.GetUser()->ShowHistory();
                    else if(c==6) auth.Logout();
                }
            }catch(exception& e){ cerr<<"Main loop error: "<<e.what()<<"\n"; }
            this_thread::sleep_for(milliseconds(100));
        }

        // Cleanup
        delete tomato; delete cheese; delete chicken;
        delete f1; delete f2; delete f3;
        delete m1; delete m2; delete m3;
    }catch(exception& e){ cerr<<"Fatal error: "<<e.what()<<"\n"; }
    return 0;
}