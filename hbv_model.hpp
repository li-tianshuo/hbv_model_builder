// Copyright 2022 Tianshuo Li
#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
/**
 * @brief hbv_model class will build HBV model with given dataset and parameters.
 * It included the function that can calculated predictions value and NSE, and provide 
 * function with return value.
 */
class hbv_model {
 public:
      /**
       * @brief This vector are use to storing Q(runoff/discharge) from dataset.
       */
      std::vector<double> Q;
      /**
       * @brief This vector are use to storing P(precipitation) from dataset.
       */
      std::vector<double> P;
      /**
       * @brief This vector are use to storing T(daily mean temperature) from dataset.
       */
      std::vector<double> T;
      /**
       * @brief parameters vector are used to store parameters, and it store the parameters
       * as following:
       * parameter[0]=T_tr
       * parameter[1]=DF
       * parameter[2]=FC
       * parameter[3]=beta
       * parameter[4]=alpha
       * parameter[5]=LP
       * parameter[6]=K0
       * parameter[7]=K1
       * parameter[8]=K2
       * parameter[9]=Lsuz
       * parameter[10]=Cperc
       * parameter[11]=SD_i
       * parameter[12]=SUZ_i
       * parameter[13]=SLZ_i
       * parameter[14]=SM_i
       * parameter[15]=A
       */
      std::vector<double> parameters;
      /**
       * @brief Construct a new hbv model model
       * @param Qz discharge vector given bt dataset
       * @param P1 precipitation vector given by dataset
       * @param T1 daily mean temperature vector given by dataset
       * @param parameters1 parameters vector that included basic elements for hbv model calculations
       * Please note this constructor will not check the size of each vectors. Please check that before
       * construct hbv model.
       */
      hbv_model(const std::vector<double> &Qz, const std::vector<double> &P1,
      const std::vector<double> &T1, const std::vector<double> &parameters1) {
         Q = Qz;
         P = P1;
         T = T1;
         parameters = parameters1;
         setParameter();
         getAverageQ();
         getResult();
      }
      /**
       * @brief getNSE() will return NSE value from HBV model
       * @return double NSE value
       */
      double getNSE() {
         return NSE;
      }
      /**
       * @brief getRF() will return RF vector from HBV model
       * @return std::vector<double>  RF
       */
      std::vector<double> getRF() {
         return RF;
      }
      /**
       * @brief getET() will return ET vector from HBV model
       * @return std::vector<double> ET
       */
      std::vector<double> getET() {
         return ET;
      }
      /**
       * @brief getAET() will return AET vector from HBV model
       */
      std::vector<double> getAET() {
         return AET;
      }
      /**
       * @brief getSLZ() will return SLZ vector from HBV model
       * @return std::vector<double> SLZ
       */
      std::vector<double> getSLZ() {
         return SLZ;
      }
      /**
       * @brief getSUZ() will return SUZ vector from HBV model
       * @return std::vector<double> SUZ
       */
      std::vector<double> getSUZ() {
         return SUZ;
      }
      /**
       * @brief getSQt() will return Qt vector from HBV model
       * @return std::vector<double> Qt
       */
      std::vector<double> getQt() {
         return Qt;
      }
      /**
       * @brief getQa() will return Q_a vector from HBV model
       * @return std::vector<double> Q_a
       */
      std::vector<double> getQa() {
         return Q_a;
      }
      /**
       * @brief getNSE_AD() will overview of NSE value and suggestion based on dataset,
       * and print those content to console.
       */
      void getNSE_AD() {
         if (NSE == -2) {
            std::cout<< "The HBV model is not initialized or the value is not acceptable" <<"\n";
            std::cout<< "Please check the dataset and parameters or initialized the HBV model" <<"\n";
         } else {
            std::cout<< "The NSE value is " << NSE <<"\n\n";
            std::cout<< "NSE is a value to evaluate performance of model."
               <<"A value closer to 1 mean better performance \non the model just like R^2"
               <<"\n\n";
            std::cout<< "  Suggestion: \n";
            if (NSE == 1) {
               std::cout<< "The result seems prefect but please check if the dataset is over fitted\n";
            } else if (NSE< 1 && NSE >= 0.9) {
               std::cout<< "The result seems prefect. But over fitted maybe a potential issue \n";
            } else if (NSE< 0.9 && NSE >= 0.7) {
               std::cout<< "The result seems good. \n";
            } else if (NSE< 0.7 && NSE >= 0.4) {
               std::cout<< "The result seems acceptable. \n";
            } else if (NSE< 0.4) {
               std::cout<< "The result seems not acceptable. Please choose other model or check dataset \n";
            }
         }
      }

 private:
      /**
       * @brief NSE mean Nash–Sutcliffe model efficiency coefficient value. It's a value to 
       * evaluate performance of the hydrologic model. The value of it usually between 
       * 0 and 1(may less than 0), when it closer to 1 mean a better performance.
       */
      double NSE = -2;
      /**
       * @brief averageQ is the average of Q (runoff/discharge) value from dataset. It will use 
       * to calculate NSE value.
       */
      double averageQ;
      /**
       * @brief those value are declare as parameters value for calculation. Detailed information
       * about each parameters can be found in Readme file.
       */
      double T_tr, DF, FC, beta, alpha, LP, k0, k1, k2, Lsuz, Cperc, SD_i, SUZ_i, SLZ_i, SM_i, A;
       /**
       * @brief Q_a are used to store Q(run off/discharge) per day in that area from HBV model.
       */
      std::vector<double> Q_a;
      /**
       * @brief S_m are used to store snowmelt rate per day from HBV model.
       */
      std::vector<double> S_m;
      /**
       * @brief SD are used to store SnowDepth per day based on precipitation and temperature.
       */
      std::vector<double> SD;
      /**
       * @brief SLZ are used to store amount of groundwater in the lower reservoir per day from calculation.
       */
      std::vector<double> SLZ;
      /**
       * @brief SM are used to store actual soil moisture per day from calculation.
       */
      std::vector<double> SM;
      /**
       * @brief ASM are used to store actual snow melt per day from calculation.
       */
      std::vector<double> ASM;
      /**
       * @brief RF are used to store effective rainfall per day from calculation.
       */
      std::vector<double> RF;
      /**
       * @brief ET are used to store potential evapotranspiration per day from calculation.
       */
      std::vector<double> ET;
      /**
       * @brief AET are used to store actual evapotranspiration per day from calculation.
       */
      std::vector<double> AET;
      /**
       * @brief F are used to store water flex per day from calculation.
       */
      std::vector<double> F;
      /**
       * @brief SUZ are used to store amount of groundwater in the upper reservoir per day from calculation.
       */
      std::vector<double> SUZ;
      /**
       * @brief Q0 are used to store amount of surface flow response per day.
       */
      std::vector<double> Q0;
      /**
       * @brief Q1 are used to store amount of interflow response per day.
       */
      std::vector<double> Q1;
      /**
       * @brief Q2 are used to store amount of base flow response per day.
       */
      std::vector<double> Q2;
      /**
       * @brief Qt are total amount of flow(runoff/discharge).
       */
      std::vector<double> Qt;
      /**
       * @brief Get the Average value of Q(run off/discharge).
       */
      void getAverageQ() {
         double a = 0;  // used to store total value of Q.
         for (size_t i = 1; i < Q.size(); i++) {
            a += Q[i];
         }
         averageQ = a/static_cast<double>(Q.size());
      }
      /**
       * @brief Start to do calculation of HBV model.
       * For detailed information about HBV equation, please check readme file.
       */
      void getResult() {
         double SG = 0;   // SG as snow gain in calculation.
         double temp1 = 0;
         // temp1 is to store the part 1 value of NSE value.
         double temp2 = 0;
         // temp2 is to store the part 2 value of NSE value.(Detail information can be found in readme file)
         SD.push_back(SD_i);
         SLZ.push_back(SLZ_i);
         SUZ.push_back(SUZ_i);
         SM.push_back(SM_i);
         for (size_t i = 0; i < Q.size(); i++) {
            SG = 0;
            if (T[i] > T_tr) {
               S_m.push_back(DF * (T[i] - T_tr));
            } else {
               S_m.push_back(0);
            }
            if (S_m[i] > SD[i]) {
               ASM.push_back(SD[i]);
            } else {
               ASM.push_back(S_m[i]);
            }
            if (T[i] < T_tr) {
               SG = P[i];
               RF.push_back(0);
            } else {
               SG = 0;
               RF.push_back(P[i]);
            }
            if (i+1 < Q.size()) {
               SD.push_back(SD[i] + SG - ASM[i]);
            }
            if (T[i] >= 0) {
               ET.push_back(alpha * T[i]);
            } else {
               ET.push_back(0);
            }
            if (i > 0) {
               AET.push_back(ET[i]* std::min(((SM[i-1]) / (FC*LP)), 1.0));
               F.push_back((pow(SM[i-1] / FC, beta)) * (RF[i] + ASM[i]));
               SM.push_back(SM[i-1] + RF[i] + ASM[i] - AET[i-1] - F[i-1]);
               if (SUZ[i-1] > Lsuz) {
                  Q0.push_back(k0*(SUZ[i-1] - Lsuz));
               } else {
                  Q0.push_back(0);
               }
               Q1.push_back(k1*SUZ[i-1]);
               Q2.push_back(std::max(k2 * SLZ[i-1], 0.0));
               Qt.push_back(Q0[i-1]+Q1[i-1] + Q2[i-1]);
               Q_a.push_back((Qt[i-1]*0.001) * A);
               SUZ.push_back(std::max((SUZ[i-1] + F[i-1] - Q0[i-1] - Q1[i-1] - Cperc), 0.0));
               SLZ.push_back(SLZ[i-1] + std::min(Cperc, SUZ[i-1]) - Q2[i-1]);
               temp1 += pow(Q[i] - Qt[i-1], 2);
               temp2 += pow(Q[i] - averageQ, 2);
            }
         }
         NSE = 1- temp1 / temp2;
      }
      /**
       * @brief Set the parameter based on given vector and check the range of each parameters.
       */
      void setParameter() {
         T_tr = parameters[0];
         DF = parameters[1];
         FC = parameters[2];
         beta = parameters[3];
         alpha = parameters[4];
         LP = parameters[5];
         k0 = parameters[6];
         k1 = parameters[7];
         k2 = parameters[8];
         Lsuz = parameters[9];
         Cperc = parameters[10];
         SD_i = parameters[11];
         SUZ_i = parameters[12];
         SLZ_i = parameters[13];
         SM_i = parameters[14];
         A = parameters[15];
         try {
            checkRange();
         }
         catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            std::cerr << "The vale out of range is set to the lower bound value" << '\n';
         }
      }
      /**
       * @brief checkRange() will check range of all parameters. If the value is not in range,
       * it will set to lower bound and throw a domain_error.
       * For detailed range information, please check readme file.
       */
      void checkRange() {
         if (!inRange(T_tr, -1.5, 2.5)) {
            T_tr = -1.5;
            throw std::domain_error("The range of T_tr should be between -1.5 and 2.5");
         }
         if (!inRange(DF, 1, 10)) {
            DF = 1;
            throw std::domain_error("The range of DF should be between 1 and 10");
         }
         if (!inRange(FC, 50, 500)) {
            FC = 50;
            throw std::domain_error("The range of FC should be between 50 and 500");
         }
         if (!inRange(beta, 1, 6)) {
            beta = 1;
            throw std::domain_error("The range of beta should be between 1 and 6");
         }
         if (!inRange(alpha, 0.5, 1.25)) {
            alpha = 0.5;
            throw std::domain_error("The range of alpha should be between 0.5 and 1.25");
         }
         if (!inRange(LP, 0.1, 0.9)) {
            LP = 0.1;
            throw std::domain_error("The range of LP should be between 0.1 and 0.9");
         }
         if (!inRange(k0, 0.01, 0.8)) {
            k0 = 0.01;
            throw std::domain_error("The range of k0 should be between 0.01 and 0.8");
         }
         if (!inRange(k1, 0.01, 0.4)) {
            k1 = 0.01;
            throw std::domain_error("The range of k1 should be between 0.01 and 0.4");
         }
         if (!inRange(k2, 0.001, 0.15)) {
            k2 = 0.001;
            throw std::domain_error("The range of k2 should be between 0.001 and 0.15");
         }
         if (!inRange(Lsuz, 1, 100)) {
            Lsuz = 1;
            throw std::domain_error("The range of Lsuz should be between 1 and 100");
         }
         if (!inRange(Cperc, 0.01, 6)) {
            Cperc = 0.01;
            throw std::domain_error("The range of Cperc should be between 0.01 and 6");
         }
      }
      /**
       * @brief This function will check value in the range.
       * @param value value want to check
       * @param low lower bound of parameters
       * @param high higher bound of parameters
       * @return true value is in the range
       * @return false value is not in the range
       */
      bool inRange(double value, double low, double high) {
         return ((low <= value) && (value <= high));
      }
};
