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
    cyc::EreignisController ereignisController;       //ErgebisController
    long int TaktTime=100;                               //Zeit zwischen jede Takt

    ApplicationOutput(graphHandler,inputData);
    //Initialisierung
    for(const Element* element : graphHandler->getAllElements()) {   //Creat Element.
          elementAnalysis.push_back(element);
    }
    for(size_t i=0;i<graphHandler->getAllNets().size();i++){       //Signal in Nets Initialisieren.
        SignalInNets.push_back(Logic::logicX);
    }
    //End Initialisierung und beginnen das Analysis
    for(std::vector<Logic> inputValues: inputData)
    {
          //Time in ErgebisController steuern
          ereignisController.goNaechstTake(TaktTime);
          //Circuit analys..Die Reihenfolge der Werte für jeden Zeitschritt ist die Gleiche wie die Reihenfolge der Input-Netze, die vom GraphHandler zurückgegeben werden.
          cyc::CircuitAnalysis(graphHandler,inputValues,TaktTime, SignalInNets, elementAnalysis,ereignisController);
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
 * @brief   Circuit analysieren durch Ereignis. Eingebe ist Inputwert in Reihenfolge. Dann wird diese Circuit durch die Inputwerten berechnet.
 * @param   *graphHandler Zeiger fuer alle Schaltungsinfomation
 * @param   Input Inputwert
 * @param   TaktTime Zeit Zwischen jede Takt
 * @param   SignalInNets Liste für Alle Wert in Netz
 * @param   elementAnalysis   Speichern die ID, Elementtype, Ein- und Ausgabewerte der Elementen.
 * @param   EreignisController Kontroller der Eieignisses
 * @return	keine
 **********************************************************************************
 */
void cyc::CircuitAnalysis(const GraphHandler* graphHandler,const std::vector<Logic> Input,const long int TaktTime,std::vector<Logic>& SignalInNets, std::vector<cyc::ElementAnalysis> &elementAnalysis,cyc::EreignisController &ereignisController){
  cyc::Ereignis ereignis;
  Logic wert;
  bool CircuitHabenClock=false;
  cyc::setPrimaryinputEreignis(graphHandler,Input,ereignisController);   //Euzeugen Ereignis durch Inputwert(whitout CLock)
  for(const Net* net: graphHandler->getAllNets()){
       if(net->getName()=="CLOCK"){
           CircuitHabenClock=true;
       }
  }
  while (1) {
      ereignis=ereignisController.getNaechstEreignis(TaktTime);
      if(ereignis.getID()==size_t(-1)){           //Keine ereignis kann be read. d.h. All Ereignisses in ereignisController wird nach nächste Takt aufgetreten oder keine Ereignis in ereignisController steht.
        break;                                     //End schleife
      }
      else{                              //-2 wird in obere gesetzt.... Es hat kein Sinn
          if(SignalInNets[ereignis.getID()]!=ereignis.getWert()){     //If Netwert geändert, dann erzeugen neue Ereignis durch neue Netwert
             // Zuerst neue Netwert ändern
            if(graphHandler->getNet(ereignis.getID())->getInElement()!=nullptr){
              elementAnalysis[graphHandler->getNet(ereignis.getID())->getInElement()->getId()].setoutputDaten(ereignis.getWert());   //Ausgangswert wird geändert
              for(const Net* net: graphHandler->getNet(ereignis.getID())->getInElement()->getOutNets()){
                  SignalInNets[net->getId()]=ereignis.getWert();
              }
            }
            else {                                                                                                     //Wert in Net wird geändert
                SignalInNets[ereignis.getID()]=ereignis.getWert();
             }
            //Dann erzeugt neue Ereignis
            if(graphHandler->getNet(ereignis.getID())->getInElement()!=nullptr){
              for(const Net* net: graphHandler->getNet(ereignis.getID())->getInElement()->getOutNets()){
                  if(net->getOutElements()[0]!=nullptr){
                     for(const Element* element:net->getOutElements()){                                    //neu Ereignis erzeugen
                        if(element->getElementInfo()->getType()==ElementType::Not){                        //neu Ereignis berechnen
                           wert=elementAnalysis[element->getId()].setinputDaten(ereignis.getWert());
                        }
                        else{
                           wert=elementAnalysis[element->getId()].setinputDaten(SignalInNets[element->getInNets()[1]->getId()],SignalInNets[element->getInNets()[0]->getId()]);
                        }
                        if(ereignisController.getWertInEreignis(element->getOutNets()[0]->getId(),ereignis.getTime())==Logic::logicError){
                            if(wert!=elementAnalysis[element->getId()].getoutputDaten())                    //nur wenn Ausgangewert geändert will, erzeugen neu Ereignis
                            ereignisController.addErgebnis(cyc::Ereignis(wert,elementAnalysis[element->getId()].getDealy(),element->getOutNets()[0]->getId()));  //Element kann mehere outNez haben. Hier erzeugt ich nur eine Ereingis einer Net. Andere wert in Net wird durch Graph geändert.
                        }
                        else{

                            if(wert!=ereignisController.getWertInEreignis(element->getOutNets()[0]->getId(),ereignis.getTime())) {                   //nur wenn Ausgangewert geändert will, erzeugen neu Ereignis
                                ereignisController.addErgebnis(cyc::Ereignis(wert,elementAnalysis[element->getId()].getDealy(),element->getOutNets()[0]->getId()));  //Element kann mehere outNez haben. Hier erzeugt ich nur eine Ereingis einer Net. Andere wert in Net wird durch Graph geändert.
                            }
                     }
                     }
                  }
              }
            }
            else {                                                                                                     //Wert in Net wird geändert
                if(graphHandler->getNet(ereignis.getID())->getOutElements()[0]!=nullptr){
                   for(const Element* element:graphHandler->getNet(ereignis.getID())->getOutElements()){                                    //neu Ereignis erzeugen
                       if(element->getElementInfo()->getType()==ElementType::Not){                                                     //neu Ereignis berechnen
                          wert=elementAnalysis[element->getId()].setinputDaten(ereignis.getWert());
                       }
                       else{
                          wert=elementAnalysis[element->getId()].setinputDaten(SignalInNets[element->getInNets()[1]->getId()],SignalInNets[element->getInNets()[0]->getId()]);
                       }
                       if(ereignisController.getWertInEreignis(element->getOutNets()[0]->getId(),elementAnalysis[element->getId()].getDealy())==Logic::logicError){
                           if(wert!=elementAnalysis[element->getId()].getoutputDaten())                    //nur wenn Ausgangewert geändert will, erzeugen neu Ereignis
                           ereignisController.addErgebnis(cyc::Ereignis(wert,elementAnalysis[element->getId()].getDealy(),element->getOutNets()[0]->getId()));  //Element kann mehere outNez haben. Hier erzeugt ich nur eine Ereingis einer Net. Andere wert in Net wird durch Graph geändert.
                       }
                       else{
                           if(wert!=ereignisController.getWertInEreignis(element->getOutNets()[0]->getId(),ereignis.getTime()))                    //nur wenn Ausgangewert geändert will, erzeugen neu Ereignis
                           ereignisController.addErgebnis(cyc::Ereignis(wert,elementAnalysis[element->getId()].getDealy(),element->getOutNets()[0]->getId()));  //Element kann mehere outNez haben. Hier erzeugt ich nur eine Ereingis einer Net. Andere wert in Net wird durch Graph geändert.
                       }
                   }
                }
             }

          }
      }

  }
  if(CircuitHabenClock){   //clock wird nur für DFF gebenutzt und es ist immer logick 1
      for(const Net* net: graphHandler->getAllNets()){
           if(net->getName()=="CLOCK"){
              for(const Element* element:net->getOutElements()){

                  wert=elementAnalysis[element->getId()].setinputDaten(SignalInNets[element->getInNets()[1]->getId()],SignalInNets[element->getInNets()[0]->getId()]);
                  if(ereignisController.getWertInEreignis(element->getOutNets()[0]->getId(),ereignis.getTime())==Logic::logicError){

                      if(wert!=elementAnalysis[element->getId()].getoutputDaten()){                    //nur wenn Ausgangewert geändert will, erzeugen neu Ereignis
                      ereignisController.addErgebnis(cyc::Ereignis(wert,elementAnalysis[element->getId()].getDealy(),element->getOutNets()[0]->getId()));  //Element kann mehere outNez haben. Hier erzeugt ich nur eine Ereingis einer Net. Andere wert in Net wird durch Graph geändert.
                      }
                   }
                  else{
                      if(wert!=ereignisController.getWertInEreignis(element->getOutNets()[0]->getId(),ereignis.getTime()))                    //nur wenn Ausgangewert geändert will, erzeugen neu Ereignis
                      ereignisController.addErgebnis(cyc::Ereignis(wert,elementAnalysis[element->getId()].getDealy(),element->getOutNets()[0]->getId()));  //Element kann mehere outNez haben. Hier erzeugt ich nur eine Ereingis einer Net. Andere wert in Net wird durch Graph geändert.
                  }
              }
           }
      }
  }
}


/*
 * @brief   erzeugt ein Ereignis durch inputwert
 * @param   *graphHandler Zeiger fuer alle Schaltungsinfomation
 * @param   inputValues Eingangswert
 * @param   EreignisController Kontroller der Eieignisses
 * @return	keine
 **********************************************************************************
 */
void cyc::setPrimaryinputEreignis(const GraphHandler* graphHandler,const std::vector<Logic> Input,cyc::EreignisController  &ereignisController){
      size_t i=0;
      size_t InputPosition=0;
      for(Logic Inputwert:Input){
          i=0;
          for(const Net* net: graphHandler->getAllNets()){
              if(net->getInElement() == nullptr){
                  if(i==InputPosition){
                  InputPosition++;
                  ereignisController.addErgebnis(cyc::Ereignis(Inputwert,0,net->getId()));
                  break;
                  }
                  i++;
              }
          }
    }
}

/*
 * @brief   erzeugt ein Ereignis durch Clock
 * @param   *graphHandler Zeiger fuer alle Schaltungsinfomation
 * @param   inputValues Eingangswert
 * @param   EreignisController Kontroller der Eieignisses
 * @return	keine
 **********************************************************************************
 */
void cyc::setClockEreignis(const GraphHandler* graphHandler,const std::vector<Logic> Input,cyc::EreignisController  &ereignisController){
    size_t i=0;
    size_t InputPosition=0;

    for(Logic Inputwert:Input){
        i=0;
        for(const Net* net: graphHandler->getAllNets()){
            if(net->getInElement() == nullptr){
                if(i==InputPosition){
                InputPosition++;
                if(net->getName()=="CLOCK")
                ereignisController.addErgebnis(cyc::Ereignis(Inputwert,0,net->getId()));
                break;
                }
                i++;
            }
        }
  }
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

