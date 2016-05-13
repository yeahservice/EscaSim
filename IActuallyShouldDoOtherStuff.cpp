#include <vector>
#include <tuple>
#include <cstddef>
#include <algorithm>   

enum class Building {None, Ressource, Producer, Booster};
enum Directions {North, NorthEast, SouthEast, South, SouthWest, NorthWest};

class HexagonField
{
	public:
	  //Use Axial coordinates, I think its called like that
    int q; //something like column
    int r; //something like row
    HexagonField()
    {
      building_ = Building::None;
    }
    
    void Build(Building b)
    {
      building_ = b;
    }
    
    void Destroy()
    {
      building_ = Building::None;
    }
  private:
    Building building_;	
};

class HexagonMap
{
  public:
    HexagonMap() //TODO throw some init map in there
    {
    }
    
    void Optimize() //TODO optimizes the internal map
    {
    }
    
    HexagonField* GetNeighbor(HexagonField *field, int direction)
    {
      int q = std::get<0>(directions[direction]);
      int r = std::get<1>(directions[direction]);
      
      return GetFieldAt((field->q + q), (field->r + r));
    }
    
    std::vector<HexagonField*> GetRing(HexagonField *center, int radius)
    {
      std::vector<HexagonField*> ring;
      
      HexagonField* ring_field = GetFieldAt(center->q, (center->r - radius));
      
      for (size_t i = 0; i < directions.size(); ++i)
      {
        for (size_t j = 0; j < radius; ++j)
        {
          ring.push_back(ring_field);
          ring_field = GetNeighbor(ring_field, i);
        }
      }
      
      return ring;
    }
    
    std::vector<HexagonField*> GetSpiral(HexagonField *center, int radius)
    {
      std::vector<HexagonField*> spiral;
      spiral.push_back(center);
      
      for (size_t i = 1; i <= radius; ++i)
      {
        std::vector<HexagonField*> ring = GetRing(center, i);
        spiral.insert(std::end(spiral), std::begin(ring), std::end(ring));
      }
      
      return spiral;
    }
    
    HexagonField* GetFieldAt(int q, int r)
    {
      return map_[r + 100][q + 100 - std::min(0, r)];
    }
    
  private:
    std::vector<HexagonField*> fields_;
    HexagonField* map_[201][201];
    
    //directions for axial coordinates
    std::vector<std::tuple<int, int>> directions { std::make_tuple(+1, 0), std::make_tuple(+1, -1), std::make_tuple(0, -1),
                                                   std::make_tuple(-1, 0), std::make_tuple(-1, +1), std::make_tuple(0, +1) };
   
};

int main()
{
	return 0;
}
