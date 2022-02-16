/*
 *  @file AnalysisEDA.h
 *
 *  @date Created on: Jun 16, 2017
 *  @author kuswik
 *
 * @par Rework
 * <table>
 * <tr><th>Date            <th>Version  <th>Author
 * <tr><td>Feb.26.2017     <td>1.0      <td>kuswik
 * <tr><td>14.11.2020      <td>1.1      <td>Yuancong CHen
 *
 */

#ifndef AnalysisEDA_H_
#define AnalysisEDA_H_

#include <vector>
#include "template.h"
#include "Graph/GraphHandler.h"
// forward declarations
class GraphHandler;
class AnalysisEDA {
public:
    AnalysisEDA(const GraphHandler* graphHandler, const std::vector<std::vector<Logic>>& inputData) :
        graphHandler(graphHandler), inputData(inputData) {
    }
    virtual ~AnalysisEDA() {
    }

    void run();

private:
    // prevent from using the default constructor, copy constructor and assignment operator
    AnalysisEDA();
    AnalysisEDA(const AnalysisEDA&);
    AnalysisEDA& operator=(const AnalysisEDA&);

    const GraphHandler* graphHandler;
    const std::vector<std::vector<Logic>>& inputData;
};

Logic operator&(const Logic &A, const Logic &B);
Logic operator|(const Logic &A, const Logic &B);
Logic operator!(const Logic &A);


namespace cyc {
    class ElementAnalysis{
    private:
        size_t ID;
        ElementType elementType;
        std::vector<Logic> inputDaten;
        Logic outputDaten;
        long int dealy;

    public:
        ElementAnalysis(const Element* element){
            ID=element->getId();
            elementType=element->getElementInfo()->getType();
            outputDaten=Logic::logicX;
            dealy=0;
            if(elementType==ElementType::Not){
                inputDaten.push_back(Logic::logicX);
            }
            else{
                if(elementType!=ElementType::Dff){                    //elementType==ElementType::Add oder Or
                    inputDaten.push_back(Logic::logicX);
                    inputDaten.push_back(Logic::logicX);
                }
                else{                                                 //elementType==ElementType::Dff
                    inputDaten.push_back(Logic::logicX);
                    inputDaten.push_back(Logic::logicX);              //ClockTakt
                    dealy=100;                                //Takttime...
                }
            }
        }

        size_t getID()const{
            return ID;
        }

        void setoutputDaten(const Logic& outputData){

            outputDaten=outputData;

        }

        Logic setinputDaten(const Logic& inputData){
            if(elementType!=ElementType::Not){
                std::cout << "falsch eingegeben" << std::endl;
                return Logic::logicError;;
            }
            else{
                inputDaten[0]=inputData;
                return !inputDaten[0];
            }
        }

        Logic setinputDaten(const Logic& inputData,const Logic& inputData2){   //inputData2 ist ClockTakt wenn elementType==ElementType::Dff
            if(elementType==ElementType::Not){
                std::cout << "falsch eingegeben" << std::endl;
                return Logic::logicError;
            }
            else {
               if(elementType!=ElementType::Dff){               //elementType==ElementType::Add oder Or
                  inputDaten[0]=inputData;
                  inputDaten[1]=inputData2;
                  if(elementType==ElementType::Or){
                      return inputDaten[0]|inputDaten[1];
                  }
                  if(elementType==ElementType::And){
                      return inputDaten[0]&inputDaten[1];
                  }
               }
               else{
                  inputDaten[0]=inputData;
                  inputDaten[1]=inputData2;                     //ClockTakt
                  if(inputDaten[1]==Logic::logic1){
                      return inputDaten[0];
                  }
                  else {
                      return outputDaten;
                  }
               }
            }
             return Logic::logicError;
        }

        long int getDealy()const{
            return dealy;
        }

        void setDealy(long int &a){
            dealy=a;
        }
        std::vector<Logic> getinputDaten()const{
           return  inputDaten;
        }


        Logic getoutputDaten()const{
           return  outputDaten;
        }


    };



    class Ereignis{
    private:
        Logic ereigniswert;
        long int Erergnistime;              // wenn wird das Ereignis nach letzt Takt aufgetreten
        size_t id;
    public:
        Ereignis(const Logic wert,const long int time, const size_t ID){
            ereigniswert=wert;
            Erergnistime=time;
            id=ID;
        }
        Ereignis(void){
            ereigniswert=Logic::logicX;
            Erergnistime=0;
            id=-1;
        }
        Ereignis(const Ereignis &a){
            ereigniswert=a.getWert();
            Erergnistime=a.getTime();
            id=a.getID();
        }
        void operator=(const Ereignis &a){
            ereigniswert=a.getWert();
            Erergnistime=a.getTime();
            id=a.getID();
        }

        Logic getWert()const{
            return ereigniswert;
        }
        void setWert(const Logic a){
            ereigniswert=a;
        }

        void setID(const size_t a){
            id=a;
        }
        size_t getID()const{
            return id;
        }
        long int getTime()const{
            return Erergnistime;
        }
        void setTIme(const long int &time){
            Erergnistime=time;
        }
        void reduzierenTime(long int time){
            Erergnistime-=time;
        }
    };


    class EreignisController{
    private:
        std::vector<Ereignis> ereignisses;

    public:
        EreignisController(){
            //kein
        }
        void addErgebnis(const Ereignis ereignis){
            if(!ereignisses.empty()){
                bool b=true;
                for(Ereignis &a:ereignisses){
                    if((a.getID()==ereignis.getID())&&(a.getTime()==ereignis.getTime())){
                        a.setWert(ereignis.getWert());
                        b = false;
                    }
                }
                if(b==true) ereignisses.push_back(ereignis);
            }
            else
            ereignisses.push_back(ereignis);
        }
        Ereignis getNaechstEreignis(const long int Takttime){        //geben das Ereignis zurück, das die kleinste Timer haben.(Timer sollen kleiner als 'Takttime' sein)
            if(ereignisses.empty()){

                return Ereignis(Logic::logicError,1,size_t(-1));
            }
            size_t ereignisID=0;
            for(size_t i=0;i<ereignisses.size();i++){
                if(ereignisses[i].getTime()<ereignisses[ereignisID].getTime()) ereignisID=i;
            }
            Ereignis NaechsteEreignis(ereignisses[ereignisID]);
            if(ereignisses[ereignisID].getTime()<Takttime){
               ereignisses.erase(ereignisses.begin()+ereignisID);
               return NaechsteEreignis;
            }
            else{

               return Ereignis(Logic::logicError,1,size_t(-1));
            }
        }
        void goNaechstTake(const long int Takttime){
            for(Ereignis &ereignis:ereignisses){
                ereignis.setTIme(ereignis.getTime()-Takttime);
            }
        }
        Logic getWertInEreignis(const size_t id,const long int Time){
            for(Ereignis a:ereignisses){
                if((a.getID()==id)&&(a.getTime()==Time)){
                   return a.getWert();
                }
            }
            return Logic::logicError;
        }
        size_t size(void){
            return ereignisses.size();
        }
    };


      void CircuitAnalysis(const GraphHandler* graphHandler,const std::vector<Logic> Input,const long int TaktTime,std::vector<Logic>& SignalInNets, std::vector<cyc::ElementAnalysis> &elementAnalysis,cyc::EreignisController &ereignisController);
      void setPrimaryinputEreignis(const GraphHandler* graphHandler,const std::vector<Logic> Input,cyc::EreignisController  &ereignisController);
      void setClockEreignis(const GraphHandler* graphHandler,const std::vector<Logic> Input,cyc::EreignisController  &ereignisController);

}


#endif /* AnalysisEDA_H_ */
