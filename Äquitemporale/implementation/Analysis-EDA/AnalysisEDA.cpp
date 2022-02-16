/*
 * @file AnalysisEDA.cpp
 *
 *
 * @brief    This file contains the implementation of the simulator.
 *
 * @author      Yuancong Chen
 *
 * @date        14.11.2020
 *
 * @par Rework
 * <table>
 * <tr><th>Date            <th>Version  <th>Author
 * <tr><td>14.11.2020     <td>1.0      <td>Yuancong Chen
 */

#include "AnalysisEDA.h"
#include "Graph/GraphHandler.h"
#include <iostream>

void ApplicationOutput(const GraphHandler* graphHandler, const std::vector<std::vector<Logic>>& inputData);
void ApplicationOutput2(const GraphHandler* graphHandler,const std::vector<Logic> inputValues,const std::vector<Logic> SignalInNets);
void ApplicationOutput3(const std::vector<std::vector<Logic>> outputData);
/*
 * @brief   publik funktion in Classe AnalysisEDA. Schlautng analysieren
 * @param   keine
 * @return	keine
 **********************************************************************************
 */
void AnalysisEDA::run() {

    std::vector<std::vector<Logic>> outputData;    //Speichern Ausgangsdaten
    std::vector<cyc::ElementAnalysis> elementAnalysis;  //1:Speichern die ID, Elementtype, Ein- und Ausgabewerte der Elementen. 2: rechnen die Ausganswerte.
    std::vector<Logic> outputValues;               //Speichern Ausgangsdaten
    std::vector<Logic> SignalInNets;                 //Speichern die Signal in Nets

    ApplicationOutput(graphHandler,inputData);

    for(const Element* element : graphHandler->getAllElements()) {   //Creat Element.
          elementAnalysis.push_back(element);
    }

    for(size_t i=0;i<graphHandler->getAllNets().size();i++){       //Signal in Nets Initialisieren.
        SignalInNets.push_back(Logic::logicX);
    }

    for(std::vector<Logic> inputValues: inputData){
          Logic altewert;
          int genzwert=1000;              //genzwert wird für Schaltungsberechnen, um die LogicX zu finden.
          int schleifeZahl=0;             //schleifeZahl wird für Schaltungsberechnen, um die LogicX zu finden.
          bool setPrimaryinput=0;
          int EndFlag=0;                   //EndFlag für Ausgabewerte berechnen.

          //Brechnen die Ausgangswerte der DFFs
          for(const Element* element : graphHandler->getAllElements()){
              if(element->getElementInfo()->getType()==ElementType::Dff){
                   elementAnalysis[element->getId()].setinputDaten(SignalInNets[element->getInNets()[1]->getId()],SignalInNets[element->getInNets()[0]->getId()]);  //SignalInNets[element->getInNets()[0]->getId() Clock
              }
          }
          schleifeZahl=0;
          EndFlag=0;
          //Schleife für Schaltungssimulation. Schlatung
          while(!EndFlag){
                EndFlag=1;                //Diese Schleife wird geendet, wenn kein Signal geändert wird.
                schleifeZahl++;           //schleifeZahl wird für Schaltungsberechnen, um die LogicX zu finden.
                //setzen Signal (durch primary input) in Nets (nur einmal)
                if(!setPrimaryinput){
                   cyc::setPrimaryinputInNet(graphHandler,inputValues,SignalInNets);
                   EndFlag=0;
                   setPrimaryinput=true;         // Signal (primary input) in Nets (nur einmal) wird gesetzet.
                }
                //setzen Signal (durch Ausgangswert der Elemente) in Nets
                if(cyc::setAusgangswertInNet(graphHandler,elementAnalysis,SignalInNets)){
                    EndFlag=0;                          //Schaltung wird geaendert.
                }
                //Berechnen Ausgangwerte jede Element(Ausser DFF) durch Signale in Nets
                for(const Element* element : graphHandler->getAllElements()){
                    if(element->getElementInfo()->getType()!=ElementType::Dff){
                        altewert=elementAnalysis[element->getId()].getoutputDaten();
                        if(element->getElementInfo()->getType()==ElementType::Not){
                            elementAnalysis[element->getId()].setinputDaten(SignalInNets[element->getInNets()[0]->getId()]);         //Ausgangswert wird durch Private Funkion run() in CLasse cyc::ElementAnalysis nach Wertsetzung automatisch machen. fuer DFF wird es testet,
                        }                                                                                                            //ob es Rise oder Fall ist. Wenn man diese Verzögerungen der Gatter nicht vernachlässig, kann man eine Timer hier benetzen, glaube ich. Hier gibt kein enum class fur TransitionType
                        else{            //element->getElementInfo()->getType()!=ElementType::Not
                            elementAnalysis[element->getId()].setinputDaten(SignalInNets[element->getInNets()[0]->getId()],SignalInNets[element->getInNets()[1]->getId()]);
                        }
                        if(altewert!=elementAnalysis[element->getId()].getoutputDaten()){
                            EndFlag=0;
                        }
                        if((altewert!=elementAnalysis[element->getId()].getoutputDaten())&&(schleifeZahl>=genzwert)){
                            elementAnalysis[element->getId()].setoutputDaten(Logic::logicX);
                            schleifeZahl=0;
                            EndFlag=0;
                        }
                    }
                }
          }



          //speicher Ausgangswert.
          for(const Net* net: graphHandler->getAllNets()){
              if (net->getOutElements()[0] == nullptr){
                  outputValues.push_back(SignalInNets[net->getId()]);
              }
          }
          outputData.push_back(outputValues);
          outputValues.clear();

     ApplicationOutput2(graphHandler,inputValues,SignalInNets);

    }

     ApplicationOutput3(outputData);
}

/*
* @brief   Ausgabe in Application
* @param   *graphHandler Zeiger fuer alle Schaltungsinfomation
* @param   inputValues Inputwert
* @return	keine
*  **********************************************************************************
*/
void ApplicationOutput(const GraphHandler* graphHandler, const std::vector<std::vector<Logic>>& inputData){
    for (const Element* element : graphHandler->getAllElements()) {
        std::cout << element->getName() << " Position " <<element->getId() << std::endl;
    }

    // Iterate all nets:
    for(const Net* net: graphHandler->getAllNets()) {
        std::cout << net->getName() << " Position " << net->getId();
        if (net->getInElement() == nullptr)
            std::cout << " (primary input)";
        if (net->getOutElements()[0] == nullptr)
            std::cout << " (primary output)";
        std::cout << std::endl;
    }
    std::cout << "input"<< std::endl;
    // Iterate all time steps:
    for (const std::vector<Logic>& timeStep : inputData) {
        for (const Logic& value : timeStep) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }



    std::cout << std::endl;
    std::cout << "TaktZahl Input:";
    for(const Net* net: graphHandler->getAllNets()) {
        if (net->getInElement() == nullptr)
            std::cout << net->getName() <<"   ";
    }
    std::cout << " Netwert:";
    for(const Net* net: graphHandler->getAllNets()) {
        if (net->getName() == "CLOCK")
            std::cout << net->getName() <<"  ";
        else
            std::cout << net->getName() <<"   ";
    }

    std::cout << "Output:";
    for(const Net* net: graphHandler->getAllNets()) {
        if (net->getOutElements()[0] == nullptr)
            std::cout << net->getName() <<"  ";
    }
    std::cout << std::endl;
}

/*
 * @brief   Ausgabe in Application
 * @param   *graphHandler Zeiger fuer alle Schaltungsinfomation
 * @param   inputValues Inputwert
 * @param   SignalInNets Liste für Alle Wert in Netz
 * @return	keine
 **********************************************************************************
 */
void ApplicationOutput2(const GraphHandler* graphHandler,const std::vector<Logic> inputValues,const std::vector<Logic> SignalInNets){
    static int i=0;
    //Eingangswert, Signale in Nets und Ausgangswert ausgeben.
    if(i<10)
    std::cout << "Takt0" << i << "         ";
    else
    std::cout << "Takt" << i << "         ";
    i++;
    for (const Logic& value : inputValues) {
        std::cout << value << " ";
    }
    std::cout << "         ";
    for (const Logic& value : SignalInNets) {
        std::cout << value << " ";
    }
     std::cout <<"       ";
    for(const Net* net: graphHandler->getAllNets()) {
        if (net->getOutElements()[0] == nullptr)
            std::cout << SignalInNets[net->getId()] <<"  ";
    }

    std::cout << std::endl;
}
/*
* @brief   Ausgabe in Application
* @param   outputData Outputwert
* @return	keine
*  **********************************************************************************
*/
void ApplicationOutput3(const std::vector<std::vector<Logic>> outputData){
    std::cout << std::endl <<"Output "<< std::endl;
    for(const std::vector<Logic> &outputValues:outputData){
        for(const Logic &value:outputValues){
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}

/*
 * @brief   berechnen die Signale(logik) in Nets durch Eingangswert
 * @param   *graphHandler Zeiger fuer alle Schaltungsinfomation
 * @param   inputValues Eingangswert
 * @param   SignalInNets Speicher fuer die Signiale in Nets
 * @return	keine
 **********************************************************************************
 */
void cyc::setPrimaryinputInNet(const GraphHandler* graphHandler,const std::vector<Logic> inputValues,std::vector<Logic>& SignalInNets){
    int InputZahl=0;
    int i;
    for(const Logic Primaryinput: inputValues){
      i=0;
      for(const Net* net: graphHandler->getAllNets()){
          if(net->getInElement() == nullptr){
              if(i==InputZahl){
              SignalInNets[static_cast<int>(net->getId())]=Primaryinput;
              InputZahl++;
              break;
              }
              else i++;
          }
      }
     }
}

/*
 * @brief   berechnen die Signale(logik) in Nets durch Ausgangswert der Elementen
 * @param   *graphHandler Zeiger fuer alle Schaltungsinfomation
 * @param   elementAnalysis Speicher fuer die ID, Elementtype, Ein- und Ausgabewerte der Elementen.
 * @param   SignalInNets Speicher fuer die Signiale in Nets
 * @return	ob Signale in Nets geaendert
 **********************************************************************************
 */

bool cyc::setAusgangswertInNet(const GraphHandler* graphHandler,const std::vector<cyc::ElementAnalysis> elementAnalysis,std::vector<Logic>& SignalInNets){
    bool Signalgeandert=false;
    for(const Net* net: graphHandler->getAllNets()){
        if(net->getInElement() != nullptr){
           if(SignalInNets[net->getId()]!=elementAnalysis[net->getInElement()->getId()].getoutputDaten()){
                  SignalInNets[net->getId()]=elementAnalysis[net->getInElement()->getId()].getoutputDaten();
                  Signalgeandert=true;
           }
        }
    }
    return Signalgeandert;
}

/*
 * @brief   operator & overload fur AND fur Type Logic
 * @param   A eingangswert
 * @param   B eingangswert
 * @return	berechnet wert
 **********************************************************************************
 */
Logic operator&(const Logic &A, const Logic &B){
    if((A==Logic::logic0)&&(B==Logic::logic0))     //ein00
    return Logic::logic0;                          //aus0
    if((A==Logic::logic0)&&(B==Logic::logic1))     //ein01
    return Logic::logic0;                          //aus0
    if((A==Logic::logic0)&&(B==Logic::logicX))     //ein0X
    return Logic::logic0;                          //aus0
    if((A==Logic::logic1)&&(B==Logic::logic0))     //ein10
    return Logic::logic0;                          //aus0
    if((A==Logic::logic1)&&(B==Logic::logic1))     //ein11
    return Logic::logic1;                          //aus1
/*
    if((A==Logic::logic1)&&(B==Logic::logicX))     //ein1X
    return Logic::logicX;                          //ausX
*/
    if((A==Logic::logicX)&&(B==Logic::logic0))     //einX0
    return Logic::logic0;                          //aus0
/*
    if((A==Logic::logicX)&&(B==Logic::logic1))     //einX1
    return Logic::logicX;                          //ausX
    if((A==Logic::logicX)&&(B==Logic::logicX))     //einXX
    return Logic::logicX;                          //ausX
*/
    return Logic::logicX;
}

/*
 * @brief   operator | overload fur OR fur Type Logic
 * @param   A eingangswert
 * @param   B eingangswert
 * @return	berechnet wert
 **********************************************************************************
 */
Logic operator|(const Logic &A, const Logic &B){
    if((A==Logic::logic0)&&(B==Logic::logic0))     //ein00
    return Logic::logic0;                          //aus0
    if((A==Logic::logic0)&&(B==Logic::logic1))     //ein01
    return Logic::logic1;                          //aus1
/*
    if((A==Logic::logic0)&&(B==Logic::logicX))     //ein0X
    return Logic::logic0;                          //ausX
*/
    if((A==Logic::logic1)&&(B==Logic::logic0))     //ein10
    return Logic::logic1;                          //aus1
    if((A==Logic::logic1)&&(B==Logic::logic1))     //ein11
    return Logic::logic1;                          //aus1
/*
    if((A==Logic::logic1)&&(B==Logic::logicX))     //ein1X
    return Logic::logicX;                          //ausX
    if((A==Logic::logicX)&&(B==Logic::logic0))     //einX0
    return Logic::logic0;                          //ausX
    if((A==Logic::logicX)&&(B==Logic::logic1))     //einX1
    return Logic::logicX;                          //ausX
    if((A==Logic::logicX)&&(B==Logic::logicX))     //einXX
    return Logic::logicX;                          //ausX
*/
    return Logic::logicX;
}

/*
 * @brief   operator ! overload fur NOT fur Type Logic
 * @param   A eingangswert
 * @return	berechnet wert
 **********************************************************************************
 */
Logic operator!(const Logic &A){
    if(A==Logic::logic0)                   //ein0
    return Logic::logic1;
    if(A==Logic::logic1)                   //ein1
    return Logic::logic0;
    return Logic::logicX;                  //einX
}

