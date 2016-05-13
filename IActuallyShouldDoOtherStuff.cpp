#include <vector>
#include <tuple>
#include <cstddef>
#include <iostream>
#include <algorithm>   

enum class Building {None, Ressource, Producer, Booster};
enum Directions {North, NorthEast, SouthEast, South, SouthWest, NorthWest};

template<typename T>
std::ostream& operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& stream, const T& e)
{
    return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

class HexagonField
{
	public:
	  //Use Axial coordinates, I think its called like that
    int q_; //something like column
    int r_; //something like row
    
    Building building_;	
    
    HexagonField(int q, int r) : q_(q), r_(r), building_(Building::None)
    {
    }
    
    void Build(Building b)
    {
      building_ = b;
    }
    
    void Destroy()
    {
      building_ = Building::None;
    }
};

class HexagonMap
{
  public:
    
    std::vector<HexagonField*> fields_;
    
    HexagonMap(int radius)
    {
      for (int q = 0; q < 201; ++q)
      {
        for (int r = 0; r < 201; ++r)
        {
          map_[q][r] = NULL;
        }
      }
      
      //add center field and then each ring
      HexagonField *center = AddNewField(0, 0);
      
      for (int i = 1; i <= radius; ++i)
      {
        std::vector<std::tuple<int, int>> ring_coords = GetRingCoords(center, i);
        std::vector<std::tuple<int, int>>::iterator it;
        for (it = ring_coords.begin(); it != ring_coords.end(); ++it)
        {
          AddNewField(std::get<0>(*it), std::get<1>(*it));
        }
      }
      
      PrintMap();
      
    }
    
    HexagonField* AddNewField(int q, int r)
    {
      HexagonField *field = new HexagonField(q, r);
      HexagonField *current = GetFieldAt(field->q_, field->r_);
      if (current != NULL)
      {
        //TODO remove from map and fields
        delete current;
        current = NULL;
      }
      
      map_[field->q_ + 100][field->r_ + 100 - std::min(0, field->q_)] = field;
      fields_.push_back(field);
      
      return field;
    }
    
    HexagonField* GetNeighbor(HexagonField *field, int direction)
    {
      int q = std::get<0>(directions[direction]);
      int r = std::get<1>(directions[direction]);
      
      //std::cout << "Get Neighbor with q: " << q << " r: " << r << " direction: " << direction << std::endl;
            
      return GetFieldAt((field->q_ + q), (field->r_ + r));
    }
    
    std::tuple<int, int> GetNeighborCoords(int field_q, int field_r, int direction)
    {
      int q = std::get<0>(directions[direction]);
      int r = std::get<1>(directions[direction]);
      
      //std::cout << "Get Neighbor coords with q: " << q << " r: " << r << " direction: " << direction << std::endl;
      
      return std::make_tuple((field_q + q), (field_r + r));
    }
    
    std::vector<HexagonField*> GetRing(HexagonField *center, int radius)
    {
      //std::cout << "Get ring with q: " << center->q_ << " r: " << center->r_ << " radius: " << radius << std::endl;
      
      std::vector<HexagonField*> ring;
      int q = center->q_ - radius;
      int r = center->r_ + radius;
      
      //std::cout << "Starting field q: " << q << " r: " << r << std::endl;
      
      for (size_t i = 0; i < directions.size(); ++i)
      {
        for (size_t j = 0; j < radius; ++j)
        {
          HexagonField* ring_field = GetFieldAt(q, r);
          
          //std::cout << "At ring field q: " << ring_field->q_ << " r: " << ring_field->r_ << std::endl; 
          
          if (ring_field != NULL)
          {
            ring.push_back(ring_field);
          }
          
          std::tuple<int, int> neighboor_coords = GetNeighborCoords(q, r, i);
          q = std::get<0>(neighboor_coords);
          r = std::get<1>(neighboor_coords);
        }
      }
      
      //std::cout << std::endl;
      
      return ring;
    }
    
    std::vector<std::tuple<int, int>> GetRingCoords(HexagonField *center, int radius)
    {
      std::vector<std::tuple<int, int>> ring_coords;
      int q = center->q_ - radius;
      int r = center->r_ + radius;

      for (size_t i = 0; i < directions.size(); ++i)
      {
        for (size_t j = 0; j < radius; ++j)
        {
          ring_coords.push_back(std::make_tuple(q, r));
          
          std::tuple<int, int> neighboor_coords = GetNeighborCoords(q, r, i);
          q = std::get<0>(neighboor_coords);
          r = std::get<1>(neighboor_coords);
        }
      }
      
      return ring_coords;
    }
    
    std::vector<HexagonField*> GetSpiral(HexagonField *center, int radius)
    {
      std::vector<HexagonField*> spiral;
      
      for (size_t i = 1; i <= radius; ++i)
      {
        std::vector<HexagonField*> ring = GetRing(center, i);
        spiral.insert(std::end(spiral), std::begin(ring), std::end(ring));
      }
      
      return spiral;
    }
    
    HexagonField* GetFieldAt(int q, int r)
    {
      //std::cout << "Get Field at q:" << q << " r: " << r << std::endl;
      return map_[q + 100][r + 100 - std::min(0, q)];
    }
    
    int CalcTotalMapProduction()
    {
      int total_prod = 0;
      std::vector<HexagonField*>::iterator fields_it;
      
      for (fields_it = fields_.begin(); fields_it != fields_.end(); ++fields_it)
      {
        if ((*fields_it)->building_ == Building::Producer)
        {
          int prods = 0;
          int boosts = 1;
          
          std::vector<HexagonField*> area_of_influence = GetSpiral(*fields_it, 3);
          std::vector<HexagonField*>::iterator area_of_influence_it;

          for (area_of_influence_it = area_of_influence.begin(); area_of_influence_it != area_of_influence.end(); ++area_of_influence_it)
          {
            //std::cout << "field in influence q: " << (*area_of_influence_it)->q_ << " r: " << (*area_of_influence_it)->r_ << std::endl;
            if ((*area_of_influence_it)->building_ == Building::Ressource) prods++;
            if ((*area_of_influence_it)->building_ == Building::Booster) boosts++;
          }
          //boosts = 100% on base prod
          total_prod = total_prod + ((30 * boosts) * prods);
        }
      }
      
      return total_prod;
    }
    
    void PrintMap()
    {
      std::vector<HexagonField*>::iterator fields_it;
      
      for (fields_it = fields_.begin(); fields_it != fields_.end(); ++fields_it)
      {
        std::cout << (*fields_it)->q_ << (*fields_it)->r_ << ": " << (*fields_it)->building_ << std::endl;
      }
      std::cout << std::endl;
    }
    
  private:
    HexagonField* map_[201][201];
    
    //directions for axial coordinates
    std::vector<std::tuple<int, int>> directions { std::make_tuple(0, -1), std::make_tuple(+1, -1), std::make_tuple(+1, 0), 
                                                   std::make_tuple(0, +1), std::make_tuple(-1, +1), std::make_tuple(-1, 0) };
   
};

class Simulator
{
  public:
    Simulator()
    {
      
    }
    
    void Optimize (HexagonMap *map)
    {
      RecursiveOptimize(map, 0, 0);
    }
    
    private:
      int RecursiveOptimize(HexagonMap *map, int index, int highest_prod)
      {
        int prod = 0;
        
        map->fields_[index]->Build(Building::Ressource);
        if ((index + 1) < map->fields_.size())
        {
          highest_prod = RecursiveOptimize(map, index + 1, highest_prod);
        }
        else
        {
          prod = map->CalcTotalMapProduction();
          if (highest_prod < prod)
          {
            highest_prod = prod;
            std::cout << "Found higher total prod: " << highest_prod << std::endl;
            map->PrintMap();
          }
        }
        
        map->fields_[index]->Build(Building::Producer);
        if ((index + 1) < map->fields_.size())
        {
          highest_prod = RecursiveOptimize(map, index + 1, highest_prod);
        }
        else
        {
          prod = map->CalcTotalMapProduction();
          if (highest_prod < prod)
          {
            highest_prod = prod;
            std::cout << "Found higher total prod: " << highest_prod << std::endl;
            map->PrintMap();
          }
        }
        
        map->fields_[index]->Build(Building::Booster);
        if ((index + 1) < map->fields_.size())
        {
          highest_prod = RecursiveOptimize(map, index + 1, highest_prod);
        }
        else
        {
          prod = map->CalcTotalMapProduction();
          if (highest_prod < prod)
          {
            highest_prod = prod;
            std::cout << "Found higher total prod: " << highest_prod << std::endl;
            map->PrintMap();
          }
        }
          
        return highest_prod;
      }
};

int main()
{
  HexagonMap *map = new HexagonMap(2);
  //std::cout << "Total production: " << map.CalcTotalMapProduction() << std::endl;
  //std::cout << "Total production: " << map.CalcTotalMapProduction() << std::endl;
  
  Simulator sim;
  sim.Optimize(map);
  
	return 0;
}
