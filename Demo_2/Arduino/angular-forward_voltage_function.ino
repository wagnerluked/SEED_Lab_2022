//Prototypes
float angular();

float forward();

//Angular Func to return angular voltage
float angular(){
 //Calculates phiError   
    phiError = phiTarget - phi; //rads

    //Calculates integal term
    phiI = phiI + (Ts * phiError);
    if(abs(phiError) <= 0.01) phiI = 0;
    
    //Calculates desired angular velocity input
    phi_dotTarget = (phiKp * phiError) + (phiKi * phiI);
    
    //Increases phi_dot slowly to eliminate jumping
    if(phi_dotIncrement <= 20) {
        phi_dotTarget = phi_dotTarget * 0.05 * phi_dotIncrement;
        phi_dotIncrement++;
    }

    //Calculates phi_dotError
    phi_dotError = phi_dotTarget - phi_dot; //rads/sec

    angularVolt = (phi_dotKp * phi_dotError); //volts

    Serial.println(angularVolt);

    return angularVolt;
}

//Forward Func to return forward voltage
float forward(){
   //Sets a forward velocity once the phi has reached its target
     if(forwardFlag == true && killFlag == false) { //turns on forward velocity
         rho_dotTarget = 12;
        //Increases rho_dot slowly to eliminate jumping
        if(rho_dotIncrement <= 20) {
            rho_dotTarget = rho_dotTarget * 0.05 * rho_dotIncrement;
            rho_dotIncrement++;
        }
        //if(rho >= 0.99 * rhoTarget) rho_dotTarget = 0; **************************************************************************************
    
      //Calculates rho_dotError
        rho_dotError = rho_dotTarget - rho_dot; //rads/sec
        forwardVolt = (rho_dotKp * rho_dotError);
      } else if(killFlag == true) { //Turns off forward velocity
        rho_dotTarget = 0;
        //Calculates rho_dotError
        rho_dotError = rho_dotTarget - rho_dot; //rads/sec
        forwardVolt = (rho_dotKp * rho_dotError);

        //resets flags
        forwardFlag = false;
        dataReceived = false;

        Serial.println(forwardVolt);

        return forwardVolt;

      }
  }
