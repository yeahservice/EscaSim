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
    
    HexagonMap(int rings)
    {
      for (int q = 0; q < 201; ++q)
      {
        for (int r = 0; r < 201; ++r)
        {
          map_[q][r] = NULL;
        }
      }
      
      //add center field
      AddNewField(0, 0);
      AddNewField(0, -1);
      AddNewField(1, -1);
      AddNewField(1, 0);
      AddNewField(0, 1);
      AddNewField(-1, 1);
      AddNewField(-1, 0);
      
      for (int ring = 0; ring < rings; ++ring)
      {
        
      }
      
      /*
      //test setup simple field
      HexagonField *field = new HexagonField(0, 0);
      AddField(field);
      field->Build(Building::Producer);
      
      //add one ressource in influence area
      field = new HexagonField(0, -1);
      AddField(field);
      field->Build(Building::Ressource);
      
      //add one booster in influence area
      field = new HexagonField(0, 1);
      AddField(field);
      field->Build(Building::Booster);
      
      //add second ressource in influence area
      field = new HexagonField(1, -1);
      AddField(field);
      field->Build(Building::Ressource);
      */
    }
    
    void AddNewField(int q, int r)
    {
      HexagonField *field = new HexagonField(q, r);
      HexagonField *current = GetFieldAt(field->q_, field->r_);
      if (current != NULL)
      {
        delete current;
        current = NULL;
      }
      
      map_[field->q_ + 100][field->r_ + 100 - std::min(0, field->q_)] = field;
      fields_.push_back(field);
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
      bool is_dummy = false;
      
      HexagonField* ring_field = GetFieldAt(center->q_ - radius, center->r_ + radius);
      
      if (ring_field == NULL)  //hack for empty fields
      {
        ring_field = new HexagonField(center->q_ - radius, center->r_ + radius);
        is_dummy = true;
        //std::cout << "Created dummy field q: " << ring_field->q_ << " r: " << ring_field->r_ << std::endl; 
      }
      
      //std::cout << "Starting field q: " << ring_field->q_ << " r: " << ring_field->r_ << std::endl;
      
      for (size_t i = 0; i < directions.size(); ++i)
      {
        for (size_t j = 0; j < radius; ++j)
        {
          HexagonField* neighbor;
          
          //std::cout << "At ring field q: " << ring_field->q_ << " r: " << ring_field->r_ << std::endl; 
          
          if (!is_dummy)
          {
            ring.push_back(ring_field);
          }
          
          //TODO if the last neighbor created in loop is a dummy it is not cleaned up
          neighbor = GetNeighbor(ring_field, i);
          
          if (neighbor == NULL) //create dummy field
          {
            std::tuple<int, int> new_coords = GetNeighborCoords(ring_field->q_, ring_field->r_, i);
            
            if (is_dummy) //clean up old dummies not needed anymore
            {
              delete ring_field;
              ring_field = NULL;
            }
            
            ring_field = new HexagonField(std::get<0>(new_coords), std::get<1>(new_coords));
            is_dummy = true;
            //std::cout << "Created dummy field q: " << ring_field->q_ << " r: " << ring_field->r_ << std::endl; 
          }
          else
          {
            ring_field = neighbor;
            is_dummy = false;
          }
        }
      }
      
      //std::cout << std::endl;
      
      return ring;
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
    
    void Optimize (HexagonMap& map)
    {
      RecursiveOptimize(map, 0, 0);
    }
    
    private:
      int RecursiveOptimize(HexagonMap& map, int index, int highest_prod)
      {
        int prod = 0;
        
        map.fields_[index]->Build(Building::Ressource);
        if ((index + 1) < map.fields_.size())
        {
          highest_prod = RecursiveOptimize(map, index + 1, highest_prod);
        }
        else
        {
          prod = map.CalcTotalMapProduction();
          if (highest_prod < prod)
          {
            highest_prod = prod;
            std::cout << "Found higher total prod: " << highest_prod << std::endl;
            map.PrintMap();
          }
        }
        
        map.fields_[index]->Build(Building::Producer);
        if ((index + 1) < map.fields_.size())
        {
          highest_prod = RecursiveOptimize(map, index + 1, highest_prod);
        }
        else
        {
          prod = map.CalcTotalMapProduction();
          if (highest_prod < prod)
          {
            highest_prod = prod;
            std::cout << "Found higher total prod: " << highest_prod << std::endl;
            map.PrintMap();
          }
        }
        
        map.fields_[index]->Build(Building::Booster);
        if ((index + 1) < map.fields_.size())
        {
          highest_prod = RecursiveOptimize(map, index + 1, highest_prod);
        }
        else
        {
          prod = map.CalcTotalMapProduction();
          if (highest_prod < prod)
          {
            highest_prod = prod;
            std::cout << "Found higher total prod: " << highest_prod << std::endl;
            map.PrintMap();
          }
        }
          
        return highest_prod;
      }
};

int main()
{
  HexagonMap map(0);
  //std::cout << "Total production: " << map.CalcTotalMapProduction() << std::endl;
  //std::cout << "Total production: " << map.CalcTotalMapProduction() << std::endl;
  
  Simulator sim;
  sim.Optimize(map);
  
	return 0;
}
