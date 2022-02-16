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

        void run(){                                                 //Ausgangswert berechnen
            if(elementType==ElementType::Not){                      //Operator wird overloaded
                outputDaten=!inputDaten[0];
            }
            if(elementType==ElementType::Or){
                outputDaten=inputDaten[0]|inputDaten[1];
            }
            if(elementType==ElementType::And){
                outputDaten=inputDaten[0]&inputDaten[1];
            }
            if(elementType==ElementType::Dff){
                outputDaten=inputDaten[0];
            }
        }

    public:
        ElementAnalysis(const Element* element){
            ID=element->getId();
            elementType=element->getElementInfo()->getType();
            outputDaten=Logic::logicX;
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
                }
            }
        }

        size_t getID()const{
            return ID;
        }

        void setoutputDaten(const Logic& outputData){

            outputDaten=outputData;

        }

        void setinputDaten(const Logic& inputData){
            if(elementType!=ElementType::Not){
                std::cout << "falsch eingegeben" << std::endl;
            }
            else{
                inputDaten[0]=inputData;
                ElementAnalysis::run();
            }
        }
        void setinputDaten(const Logic& inputData,const Logic& inputData2){   //inputData2 ist ClockTakt wenn elementType==ElementType::Dff
            if(elementType==ElementType::Not){
                std::cout << "falsch eingegeben" << std::endl;
            }
            else {
               if(elementType!=ElementType::Dff){               //elementType==ElementType::Add oder Or
                  inputDaten[0]=inputData;
                  inputDaten[1]=inputData2;
                  ElementAnalysis::run();
               }
               else{
                  inputDaten[0]=inputData;
                  inputDaten[1]=inputData2;                     //ClockTakt
                  if(inputDaten[1]==Logic::logic1)  // Rise. Ich habe die TransitionType nicht gefunden, deswegen habe ich so geschrieben.
                      ElementAnalysis::run();
               }
            }
        }

        std::vector<Logic> getinputDaten()const{
           return  inputDaten;
        }


        Logic getoutputDaten()const{
           return  outputDaten;
        }
    };


      void setPrimaryinputInNet(const GraphHandler* graphHandler,const std::vector<Logic> inputValues,std::vector<Logic>& SignalInNets);
      bool setAusgangswertInNet(const GraphHandler* graphHandler,const std::vector<cyc::ElementAnalysis> elementAnalysis,std::vector<Logic>& SignalInNets);
}


#endif /* AnalysisEDA_H_ */
