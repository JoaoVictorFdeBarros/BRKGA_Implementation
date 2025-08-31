
#ifndef BINPACKING3D_HPP
#define BINPACKING3D_HPP

#include <vector>
#include <array>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <iostream>

struct Point3D {
    int x, y, z;

    bool operator==(const Point3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Point3D& other) const {
        return !(*this == other);
    }

    Point3D operator+(const Point3D& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Point3D operator-(const Point3D& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    bool operator<(const Point3D& other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

struct EMS {
    Point3D min_corner;
    Point3D max_corner;

    bool operator==(const EMS& other) const {
        return min_corner == other.min_corner && max_corner == other.max_corner;
    }
};

class Bin {
public:
    Point3D dimensions;
    std::vector<EMS> EMSs;
    std::vector<EMS> loaded_items;

    Bin(Point3D V);

    void update(Point3D box, EMS selected_EMS, int min_vol, int min_dim);
    bool overlapped(EMS ems1, EMS ems2);
    bool inscribed(EMS ems1, EMS ems2);
    void eliminate(EMS ems);
    std::vector<EMS> get_EMSs();
    double load();
};

class PlacementProcedure {
public:
    std::vector<Bin> Bins;
    std::vector<Point3D> boxes;
    std::vector<int> BPS;
    std::vector<double> VBO;
    int num_opened_bins;

    PlacementProcedure(const std::vector<Point3D>& input_boxes, const std::vector<Point3D>& input_bins_dims, const std::vector<double>& solution);

    void placement();
    EMS DFTRC_2(Point3D box, int k);
    Point3D orient(Point3D box, int BO);
    int select_box_orientation(double VBO_val, Point3D box, EMS ems);
    bool fit_in(Point3D box, EMS ems);
    std::pair<int, int> elimination_rule(const std::vector<Point3D>& remaining_boxes);
    double evaluate();
};

class BRKGA {
public:
    std::vector<Point3D> input_boxes;
    std::vector<Point3D> input_bins_dims;
    int N;

    int num_generations;
    int num_individuals;
    int num_gene;
    int num_elites;
    int num_mutants;
    double eliteCProb;

    double used_bins;
    double best_fitness;
    std::vector<double> best_solution;
    std::vector<double> history_min;
    std::vector<double> history_mean;

    BRKGA(const std::vector<Point3D>& input_boxes, const std::vector<Point3D>& input_bins_dims, int num_generations = 200, int num_individuals = 120, int num_elites = 12, int num_mutants = 18, double eliteCProb = 0.7);

    double decoder(const std::vector<double>& solution);
    std::vector<double> calculate_fitness(const std::vector<std::vector<double>>& population);
    void partition(const std::vector<std::vector<double>>& population, const std::vector<double>& fitness_list, std::vector<std::vector<double>>& elites, std::vector<std::vector<double>>& non_elites, std::vector<double>& elite_fitness_list);
    std::vector<double> crossover(const std::vector<double>& elite, const std::vector<double>& non_elite);
    std::vector<std::vector<double>> mating(const std::vector<std::vector<double>>& elites, const std::vector<std::vector<double>>& non_elites);
    std::vector<std::vector<double>> mutants();
    std::string fit(int patient = 4);
};

namespace InstanceGenerator {
    int ur(int lb, int ub);

    void generateInstances(int N, std::vector<Point3D>& pqr, std::vector<Point3D>& LWH,int type);
}

#endif