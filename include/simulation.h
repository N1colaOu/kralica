#include "integrators.h"
#include<memory>
#include<vector>
#include<numeric>
#include<print>
#include<string>
#include<fstream>
#include<stdexcept>

namespace kralica{
    class Simulation{
        private:
        std::vector<Body> system;
        std::vector<Vector2d> accs;
        std::unique_ptr<Integrator> integrator;
        CompositeField& force_fields;
        double dt;
        double current_time = 0.00;
        public:
        Simulation(const std::vector<Body>&,
                const std::vector<Vector2d>&, std::unique_ptr<Integrator>&, 
            CompositeField&, double);
        void step();
        void run(double);
        void run_and_log(double, const std::string&);
        void print_statistics() const;
        void log_statistics(std::ofstream&) const;
    };
    Simulation::Simulation(const std::vector<Body>& b,
            const std::vector<Vector2d>& a, std::unique_ptr<Integrator>& i, 
            CompositeField& ffs, double dt_) : 
            system(b), accs(a), integrator(std::move(i)), force_fields(ffs), dt(dt_){
                if(!integrator) throw(std::invalid_argument("Invalid Integrator"));
                if(b.size() != a.size()) throw(std::invalid_argument("Mismtach size"));
            }
    
    void Simulation::step(){
        integrator->step(system, accs, force_fields, dt);
        current_time += dt;
    }
    void Simulation::run(double end_time){
        while(current_time <= end_time){
            step();
        }
    }
    void Simulation::print_statistics() const{
        std::size_t n = system.size();      
        for (size_t i = 0; i < n; i++)
        {
            const Body& part = system[i];
            std::print("Pos: {}, Vel: {}, Acc: {}, Mass: {}", part.get_pos(), part.get_vel(), accs[i], part.get_mass());
        }
        double energy{};
        energy = std::transform_reduce(system.begin(), system.end(), 0.00, std::plus<>(),
        [](const Body& x){
            const double v = x.get_vel().norm();
            return v*v*x.get_mass()*0.50;
        });
        std::print("Total Energy: {}", energy);
    }
    void Simulation::log_statistics(std::ofstream& out) const{
        std::size_t n = system.size();      
        for (size_t i = 0; i < n; i++)
        {
            const Body& part = system[i];
            std::println(out, "{}, {}, {}, {}", part.get_pos(), part.get_vel(), accs[i], part.get_mass());
        }
    }
    void Simulation::run_and_log(double end_time, const std::string& file_name){
        std::ofstream out(file_name, std::ios::out | std::ios::trunc);
        std::println(out, "pos, vel, acc, m");
        while(current_time <= end_time){
            log_statistics(out);
            step();
        }
        log_statistics(out);
        out.close();
    }
}