#include <eosiolib/eosio.hpp>

using namespace eosio;

class [[eosio::contract]] victorytoken : public eosio::contract {

  public:
      victorytoken(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

    [[eosio::action]] void addbid(name user, int64_t price, uint64_t quantity) {
      buybids bidbook(_code, _code.value);
      print("Database loaded! #winning");
      auto iterator = bidbook.find(user.value);
      if(iterator == bidbook.end() ) {
         bidbook.emplace(get_self(), [&] (auto& bid) {
        bid.user_name = user;
        bid.price = price;
        bid.quantity = quantity;
      });
      } else {
          bidbook.modify(iterator, get_self(), [&] (auto& bid) {
              bid.price = price;
              bid.quantity = quantity;
            });
      }
     
    }
    
    [[eosio::action]] void sellpower(uint64_t quantity) {
      buybids bidbook(_code, _code.value);
      print("Database loaded! #winning");
      auto idx = bidbook.get_index<"price"_n>();
      uint64_t quantityLeft = quantity;
      for ( auto item = idx.begin(); item != idx.end(); ++item ) {
        print("quantity left: ", quantityLeft);
        print("item quantity: ", item->quantity);
        if(item->quantity <= 0) continue;
        if(quantityLeft > 0) {
          // sell
          print("quantity diff" , ((int64_t)item->quantity - (int64_t)quantityLeft));
          if((int64_t)item->quantity - (int64_t)quantityLeft <= 0) {
            quantityLeft -= item->quantity;
            idx.modify(item, get_self(), [&] (auto& bid) {
                bid.quantity = 0;
            });
          } else {
            idx.modify(item, get_self(), [&] (auto& bid) {
                bid.quantity = item->quantity - quantityLeft;
            });
                quantityLeft = 0;
          }
        } else {
          break;
        }
      }
    }
  
  
  private:

    struct [[eosio::table]] buybid {
      name user_name;
      int64_t price;
      uint64_t quantity;
      

      uint64_t primary_key() const { return user_name.value; }
      uint64_t get_price() const { return -price; }
      uint64_t get_quantity() const { return quantity; }
    };
  
    typedef eosio::multi_index<"buybids"_n, buybid, indexed_by<"price"_n, const_mem_fun<buybid, uint64_t, &buybid::get_price>>> buybids;
    
  
};

  EOSIO_DISPATCH(victorytoken, (addbid) (sellpower) )
