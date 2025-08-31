
#include "BinPacking3D.hpp"
#include <random>
#include <chrono>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <iomanip>

namespace InstanceGenerator {
    unsigned int _h48, _l48;

    int ur(int lb, int ub) {
        static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> distribution(static_cast<int>(lb), static_cast<int>(ub));
        int value = distribution(generator);
        return static_cast<int>(value >= 1 ? value : 1);
    }

    void generateInstances(int N, std::vector<Point3D>& pqr, std::vector<Point3D>& LWH,int type) {
        Point3D V = {100, 100, 100};

        switch (type)
        {
        case 6:
            V= {10,10,10};
            break;
        case 7:
            V = {40,40,40};
            break;
        default:
            break;
        }

        pqr.clear();
        LWH.clear();
        
        int Wmax = 0;
        int Wmin = 0;
        int Hmax = 0;
        int Hmin = 0;
        int Dmax = 0;
        int Dmin = 0;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> randomize(0,1);
        std::uniform_int_distribution<> randomType(1,5);

        for (int i = 0; i < N; ++i) {
            int newType = type;
            if(randomize(gen) && newType <= 5){
                newType = randomType(gen);
            }

            switch (newType)
            {
            case 1:
                Wmax = V.x/2;
                Wmin = 1;
                Hmax = V.y;
                Hmin = 2*V.y/3;
                Dmax = V.z;
                Dmin = 2*V.z/3;
                break;
            case 2:
                Wmax = V.x;
                Wmin = 2*V.x/3;
                Hmax = V.y/2;
                Hmin = 1;
                Dmax = V.z;
                Dmin = 2*V.z/3;
                break;
            case 3:
                Wmax = V.x;
                Wmin = 2*V.x/3;
                Hmax = V.y;
                Hmin = 2*V.y/3;
                Dmax = V.z/2;
                Dmin = 1;
                break;
            case 4:
                Wmax = V.x;
                Wmin = V.x/2;
                Hmax = V.y;
                Hmin = V.y/2;
                Dmax = V.z;
                Dmin = V.z/2;
                break;
            case 5:
                Wmax = V.x/2;
                Wmin = 1;
                Hmax = V.y/2;
                Hmin = 1;
                Dmax = V.z/2;
                Dmin = 1;
                break;
            case 6:
                Wmax = 10;
                Wmin = 1;
                Hmax = 10;
                Hmin = 1;
                Dmax = 10;
                Dmin = 1;
                break; 
            case 7:
                Wmax = 35;
                Wmin = 1;
                Hmax = 35;
                Hmin = 1;
                Dmax = 35;
                Dmin = 1;
                break;           
            case 8:
                Wmax = 100;
                Wmin = 1;
                Hmax = 100;
                Hmin = 1;
                Dmax = 100;
                Dmin = 1;
                break;  
            default:
                break;
            }

            Point3D new_box = {ur(Wmin, Wmax), ur(Hmin, Hmax), ur(Dmin, Dmax)};
            pqr.push_back(new_box);
        }
        for (int i = 0; i < 10000; ++i) {
            LWH.push_back(V);
        }
    }
}

Bin::Bin(Point3D V) : dimensions(V) {
    EMSs.push_back({{0, 0, 0}, V});
}

void Bin::update(Point3D box, EMS selected_EMS, int min_vol, int min_dim) {
    EMS ems_to_place = {selected_EMS.min_corner, selected_EMS.min_corner + box};
    loaded_items.push_back(ems_to_place);

    std::vector<EMS> current_EMSs = EMSs;
    for (const auto& EMS_item : current_EMSs) {
        if (overlapped(ems_to_place, EMS_item)) {
            eliminate(EMS_item);

            int x1 = EMS_item.min_corner.x, y1 = EMS_item.min_corner.y, z1 = EMS_item.min_corner.z;
            int x2 = EMS_item.max_corner.x, y2 = EMS_item.max_corner.y, z2 = EMS_item.max_corner.z;
            int x3 = ems_to_place.min_corner.x, y3 = ems_to_place.min_corner.y, z3 = ems_to_place.min_corner.z;
            int x4 = ems_to_place.max_corner.x, y4 = ems_to_place.max_corner.y, z4 = ems_to_place.max_corner.z;

            std::vector<EMS> new_EMSs_candidates = {
                {{x1, y1, z1}, {x3, y2, z2}},
                {{x4, y1, z1}, {x2, y2, z2}},
                {{x1, y1, z1}, {x2, y3, z2}},
                {{x1, y4, z1}, {x2, y2, z2}},
                {{x1, y1, z1}, {x2, y2, z3}},
                {{x1, y1, z4}, {x2, y2, z2}}
            };

            for (const auto& new_EMS : new_EMSs_candidates) {
                Point3D new_box_dims = new_EMS.max_corner - new_EMS.min_corner;
                bool isValid = true;

                for (const auto& other_EMS : EMSs) {
                    if (inscribed(new_EMS, other_EMS)) {
                        isValid = false;

                        break;
                    }
                }

                // Do not add new EMS smaller than the volume of remaining boxes
                if (new_box_dims.x < min_dim || new_box_dims.y < min_dim || new_box_dims.z < min_dim) {
                    isValid = false;
                }

                // Do not add new EMS having smaller dimension of the smallest dimension of remaining boxes
                if (static_cast<long long>(new_box_dims.x) * new_box_dims.y * new_box_dims.z < min_vol) {
                    isValid = false;
                }

                if (isValid) {
                    EMSs.push_back(new_EMS);
                }
            }
        }
    }
}

bool Bin::overlapped(EMS ems1, EMS ems2) {
    return (ems1.max_corner.x > ems2.min_corner.x && ems1.max_corner.y > ems2.min_corner.y && ems1.max_corner.z > ems2.min_corner.z) &&
           (ems1.min_corner.x < ems2.max_corner.x && ems1.min_corner.y < ems2.max_corner.y && ems1.min_corner.z < ems2.max_corner.z);
}

bool Bin::inscribed(EMS ems1, EMS ems2) {
    return (ems2.min_corner.x <= ems1.min_corner.x && ems2.min_corner.y <= ems1.min_corner.y && ems2.min_corner.z <= ems1.min_corner.z) &&
           (ems1.max_corner.x <= ems2.max_corner.x && ems1.max_corner.y <= ems2.max_corner.y && ems1.max_corner.z <= ems2.max_corner.z);
}

void Bin::eliminate(EMS ems) {
    EMSs.erase(std::remove(EMSs.begin(), EMSs.end(), ems), EMSs.end());
}

std::vector<EMS> Bin::get_EMSs() {
    return EMSs;
}

double Bin::load() {
    long long loaded_volume = 0;
    for (const auto& item : loaded_items) {
        loaded_volume += static_cast<long long>(item.max_corner.x - item.min_corner.x) *
                         (item.max_corner.y - item.min_corner.y) *
                         (item.max_corner.z - item.min_corner.z);
    }
    long long bin_volume = static_cast<long long>(dimensions.x) * dimensions.y * dimensions.z;
    return static_cast<double>(loaded_volume) / bin_volume;
}

PlacementProcedure::PlacementProcedure(const std::vector<Point3D>& input_boxes, const std::vector<Point3D>& input_bins_dims, const std::vector<double>& solution) 
    : boxes(input_boxes), num_opened_bins(1){
    
    for (const auto& dim : input_bins_dims) {
        Bins.emplace_back(dim);
    }

    std::vector<std::pair<double, int>> indexed_solution_bps(boxes.size());
    for (size_t i = 0; i < boxes.size(); ++i) {
        indexed_solution_bps[i] = {solution[i], i};
    }
    std::sort(indexed_solution_bps.begin(), indexed_solution_bps.end());
    for (const auto& p : indexed_solution_bps) {
        BPS.push_back(p.second);
    }
    for (size_t i = boxes.size(); i < solution.size(); ++i) {
        VBO.push_back(solution[i]);
    }
    placement();
}

void PlacementProcedure::placement() {
    std::vector<Point3D> items_sorted;
    for (int idx : BPS) {
        items_sorted.push_back(boxes[idx]);
    }

    for (size_t i = 0; i < items_sorted.size(); ++i) {
        Point3D box = items_sorted[i];

        int selected_bin_idx = -1;
        EMS selected_EMS = {{0,0,0},{0,0,0}};

        for (int k = 0; k < num_opened_bins; ++k) {
            EMS ems = DFTRC_2(box, k);
            if (ems.min_corner.x != 0 || ems.min_corner.y != 0 || ems.min_corner.z != 0 ||
                ems.max_corner.x != 0 || ems.max_corner.y != 0 || ems.max_corner.z != 0) {
                selected_bin_idx = k;
                selected_EMS = ems;
                break;
            }
        }

        if (selected_bin_idx == -1) {
            num_opened_bins++;
            selected_bin_idx = num_opened_bins - 1;
            selected_EMS = Bins[selected_bin_idx].EMSs[0]; // origin of the new bin
        }

        int BO = select_box_orientation(VBO[i], box, selected_EMS);

        std::vector<Point3D> remaining_boxes_slice;
        if (i + 1 < items_sorted.size()) {
            remaining_boxes_slice.assign(items_sorted.begin() + i + 1, items_sorted.end());
        }
        std::pair<int, int> elimination_vals = elimination_rule(remaining_boxes_slice);
        int min_vol = elimination_vals.first;
        int min_dim = elimination_vals.second;

        Bins[selected_bin_idx].update(orient(box, BO), selected_EMS, min_vol, min_dim);
    }
}

EMS PlacementProcedure::DFTRC_2(Point3D box, int k) {
    double maxDist = -1.0;
    EMS selectedEMS = {{0,0,0},{0,0,0}};

    for (const auto& EMS_item : Bins[k].EMSs) {
        int D_bin = Bins[k].dimensions.x;
        int W_bin = Bins[k].dimensions.y;
        int H_bin = Bins[k].dimensions.z;

        for (int direction = 1; direction <= 6; ++direction) {
            Point3D oriented_box = orient(box, direction);
            if (fit_in(oriented_box, EMS_item)) {
                int x = EMS_item.min_corner.x;
                int y = EMS_item.min_corner.y;
                int z = EMS_item.min_corner.z;
                
                double distance = std::pow(D_bin - x - oriented_box.x, 2) +
                                  std::pow(W_bin - y - oriented_box.y, 2) +
                                  std::pow(H_bin - z - oriented_box.z, 2);

                if (distance > maxDist) {
                    maxDist = distance;
                    selectedEMS = EMS_item;
                }
            }
        }
    }
    return selectedEMS;
}

Point3D PlacementProcedure::orient(Point3D box, int BO) {
    int d = box.x, w = box.y, h = box.z;
    if (BO == 1) return {d, w, h};
    else if (BO == 2) return {d, h, w};
    else if (BO == 3) return {w, d, h};
    else if (BO == 4) return {w, h, d};
    else if (BO == 5) return {h, d, w};
    else if (BO == 6) return {h, w, d};
    return {0, 0, 0};
}

int PlacementProcedure::select_box_orientation(double VBO_val, Point3D box, EMS ems) {
    std::vector<int> BOs;
    for (int direction = 1; direction <= 6; ++direction) {
        if (fit_in(orient(box, direction), ems)) {
            BOs.push_back(direction);
        }
    }

    int selectedBO = BOs[static_cast<int>(std::ceil(VBO_val * BOs.size())) - 1];
    return selectedBO;
}

bool PlacementProcedure::fit_in(Point3D box, EMS ems) {
    return (box.x <= (ems.max_corner.x - ems.min_corner.x)) &&
           (box.y <= (ems.max_corner.y - ems.min_corner.y)) &&
           (box.z <= (ems.max_corner.z - ems.min_corner.z));
}

std::pair<int, int> PlacementProcedure::elimination_rule(const std::vector<Point3D>& remaining_boxes) {
    if (remaining_boxes.empty()) {
        return {0, 0};
    }

    int min_vol = 999999999;
    int min_dim = 9999;
    for (const auto& box : remaining_boxes) {
        int dim = std::min({box.x, box.y, box.z});
        if (dim < min_dim) {
            min_dim = dim;
        }
        long long vol = static_cast<long long>(box.x) * box.y * box.z;
        if (vol < min_vol) {
            min_vol = static_cast<int>(vol);
        }
    }
    return {min_vol, min_dim};
}

double PlacementProcedure::evaluate() {

    double leastLoad = 1.0;
    for (int k = 0; k < num_opened_bins; ++k) {
        double load = Bins[k].load();
        if (load < leastLoad) {
            leastLoad = load;
        }
    }
    return static_cast<double>(num_opened_bins) + leastLoad;
}

BRKGA::BRKGA(const std::vector<Point3D>& input_boxes, const std::vector<Point3D>& input_bins_dims, int num_generations, int num_individuals, int num_elites, int num_mutants, double eliteCProb)
    : input_boxes(input_boxes), input_bins_dims(input_bins_dims),
      N(input_boxes.size()), num_generations(num_generations),
      num_individuals(num_individuals), num_gene(2 * input_boxes.size()),
      num_elites(num_elites), num_mutants(num_mutants), eliteCProb(eliteCProb),
      used_bins(-1), best_fitness(-1) {}

double BRKGA::decoder(const std::vector<double>& solution) {
    PlacementProcedure placement(input_boxes, input_bins_dims, solution);
    return placement.evaluate();
}

std::vector<double> BRKGA::calculate_fitness(const std::vector<std::vector<double>>& population) {
    std::vector<double> fitness_list;
    for (const auto& solution : population) {
        fitness_list.push_back(decoder(solution));
    }
    return fitness_list;
}

void BRKGA::partition(const std::vector<std::vector<double>>& population, const std::vector<double>& fitness_list, 
                      std::vector<std::vector<double>>& elites, std::vector<std::vector<double>>& non_elites, 
                      std::vector<double>& elite_fitness_list) {
    
    std::vector<std::pair<double, int>> indexed_fitness(fitness_list.size());
    for (size_t i = 0; i < fitness_list.size(); ++i) {
        indexed_fitness[i] = {fitness_list[i], i};
    }
    std::sort(indexed_fitness.begin(), indexed_fitness.end());

    elites.clear();
    non_elites.clear();
    elite_fitness_list.clear();

    for (int i = 0; i < num_elites; ++i) {
        elites.push_back(population[indexed_fitness[i].second]);
        elite_fitness_list.push_back(indexed_fitness[i].first);
    }

    for (size_t i = num_elites; i < population.size(); ++i) {
        non_elites.push_back(population[indexed_fitness[i].second]);
    }
}

std::vector<double> BRKGA::crossover(const std::vector<double>& elite, const std::vector<double>& non_elite) {
    std::vector<double> offspring(num_gene);
    static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    for (int i = 0; i < num_gene; ++i) {
        if (distribution(generator) < eliteCProb) {
            offspring[i] = elite[i];
        } else {
            offspring[i] = non_elite[i];
        }
    }
    return offspring;
}

std::vector<std::vector<double>> BRKGA::mating(const std::vector<std::vector<double>>& elites, const std::vector<std::vector<double>>& non_elites) {
    int num_offspring = num_individuals - num_elites - num_mutants;
    std::vector<std::vector<double>> offsprings;
    offsprings.reserve(num_offspring);

    static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> elite_dist(0, elites.size() - 1);
    std::uniform_int_distribution<int> non_elite_dist(0, non_elites.size() - 1);

    for (int i = 0; i < num_offspring; ++i) {
        const auto& elite_parent = elites[elite_dist(generator)];
        const auto& non_elite_parent = non_elites[non_elite_dist(generator)];
        offsprings.push_back(crossover(elite_parent, non_elite_parent));
    }
    return offsprings;
}

std::vector<std::vector<double>> BRKGA::mutants() {
    std::vector<std::vector<double>> new_mutants(num_mutants, std::vector<double>(num_gene));
    static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    for (int i = 0; i < num_mutants; ++i) {
        for (int j = 0; j < num_gene; ++j) {
            new_mutants[i][j] = distribution(generator);
        }
    }
    return new_mutants;
}

std::string BRKGA::fit(int patient) {
    // Initial population & fitness
    std::vector<std::vector<double>> population(num_individuals, std::vector<double>(num_gene));
    static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    for (int i = 0; i < num_individuals; ++i) {
        for (int j = 0; j < num_gene; ++j) {
            population[i][j] = distribution(generator);
        }
    }
    std::vector<double> fitness_list = calculate_fitness(population);

    double max_fitness = -1.0;
    if (!fitness_list.empty()) {
        max_fitness = *std::max_element(fitness_list.begin(), fitness_list.end());
    }
    std::cout << "\nFitness inicial: " << max_fitness << "\n\n";
    

    best_fitness = 100000; // Initialize with a high value
    if (!fitness_list.empty()) {
        best_fitness = *std::min_element(fitness_list.begin(), fitness_list.end());
        best_solution = population[std::min_element(fitness_list.begin(), fitness_list.end()) - fitness_list.begin()];
    }
    history_min.push_back(best_fitness);
    history_mean.push_back(std::accumulate(fitness_list.begin(), fitness_list.end(), 0.0) / fitness_list.size());

    // Repeat generations
    int best_iter = 0;
    std::cout << "-----------------------------PROCESSANDO-GERAÇÕES------------------------------\n";
    std::cout << "Geração->Fitness: \n\n";
    for (int g = 0; g < num_generations; ++g) {
        // early stopping
        if (g - best_iter > patient) {
            used_bins = std::floor(best_fitness);
            return "feasible";
        }

        // Select elite group
        std::vector<std::vector<double>> elites, non_elites;
        std::vector<double> elite_fitness_list;
        partition(population, fitness_list, elites, non_elites, elite_fitness_list);

        // Biased Mating & Crossover
        std::vector<std::vector<double>> offsprings = mating(elites, non_elites);

        // Generate mutants
        std::vector<std::vector<double>> new_mutants = mutants();

        // New Population & fitness
        std::vector<std::vector<double>> next_population;
        next_population.reserve(elites.size() + new_mutants.size() + offsprings.size());
        next_population.insert(next_population.end(), elites.begin(), elites.end());
        next_population.insert(next_population.end(), new_mutants.begin(), new_mutants.end());
        next_population.insert(next_population.end(), offsprings.begin(), offsprings.end());
        
        population = next_population;
        fitness_list = calculate_fitness(population);

        // Update Best Fitness
        double current_min_fitness = 100000;
        if (!fitness_list.empty()) {
            current_min_fitness = *std::min_element(fitness_list.begin(), fitness_list.end());
        }

        if (current_min_fitness < best_fitness) {
            best_iter = g;
            best_fitness = current_min_fitness;
            best_solution = population[std::min_element(fitness_list.begin(), fitness_list.end()) - fitness_list.begin()];
        }

        history_min.push_back(current_min_fitness);
        history_mean.push_back(std::accumulate(fitness_list.begin(), fitness_list.end(), 0.0) / fitness_list.size());

        std::cout << std::setw(3) << g <<"->"<< std::setw(7) <<  best_fitness<< " | " << std::flush;
        
        if(!((g+1)%5)){
            std::cout << '\n';
        }
    }

    used_bins = std::floor(best_fitness);
    return "feasible";
}


