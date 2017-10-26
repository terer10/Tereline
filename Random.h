#ifndef RANDOM_H
#define RANDOM_H
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <ctime>

template<class T>
class Random{
    public:
        Random<T>(){}
        Random<T>(T min, T max,bool repeat = true){
            _min = min;
            _max = max;
            _distribute = std::uniform_real_distribution<float>(_min,_max);
            _generator = std::mt19937(time(0) + _iterateNum);
            _repeatable = repeat;
        }
        T next(){
            //Set current number variable to the randomly generated one
            _current = static_cast<T>(_distribute(_generator));
            //If it's not allowed to repeat,
            if(!_repeatable){
                //and this number is different than the last,
                if(_current != _lastNumber){
                    //it's fine; return it.
                    return _current;
                //if the number IS THE SAME as the last,
                }else{
                    //add to the seed; change the seed
                    ++_iterateNum;
                    //reroll; recall this function
                    next();
                }
            //If it IS allowed to allowed to repeat,
            }else{
                //simply return the generated number, raw; it doesn't matter if it's a repeat
                return _current;
            }
            //Set this number as the last number
            _lastNumber = _current;
        }
        T next(T min,T max){
            _distribute = std::uniform_real_distribution<float>(min,max);
            return next();
        }
        //Set the minimum that the RNG can output
        void setMin(T min){
            _min = min;
            updateDistribute();
        }
        //Set the maximum that the RNG can output
        void setMax(T max){
            _max = max;
            updateDistribute();
        }
        //Return current number
        T getNumber(){
            return _current;
        }
    private:
        //Update the _distribute variable so that the max/min is up-to-date
        void updateDistribute(){_distribute = std::uniform_real_distribution<float>(_min,_max);}
    private:
        //Minimum number that the RNG has to generate
        T _min;
        //Max number that the RNG can generate
        T _max;
        //Current
        T _current;

        std::uniform_real_distribution<float> _distribute;
        std::mt19937 _generator;
        T _lastNumber;

        bool _repeatable = true;
        int _iterateNum = 0;
};



#endif // RANDOM_H
