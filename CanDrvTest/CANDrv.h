/** \file CANDrv.h
    \brief CANDrv header File

    This File describes the Interface of the CANDrv and the FRMMan
*/
#if defined(__AVR_ATmega16M1__)
  #define NB_MOB  6
#endif

/** \brief Baudrate
 Enumerator for the Baudrate
*/
typedef enum
{
  /**500 Kbaud*/
  CAN_500k,
  /**800 Kbaud*/
  CAN_800k
} CanBaudrate;

/** \brief MOB Operation
 Enumerator for the operation while manually using a MOB
*/
typedef enum
{
  /** Transmit DATA */
  TX_DATA = 0x01,
  /** Receive DATA */
  RX_DATA = 0x02,
  /** DISABLED */
  DISABLED = 0x00
} mob_operation;

/** \brief MOB Purpose
 Enumerator for the Purpose of the MOB when configuring the FRMMan
*/
typedef enum
{
  /**TX triggered by SW*/
  TX_DATA_SW_DRIVEN = 0x10, 
  /**RX triggered by SW*/
  RX_DATA_SW_DRIVEN = 0x20, 
  /**RX and TX triggered by SW*/
  MULTIPURPOSE = 0x30, 
  /**RX triggered by Interrupt. Automatically activated at Startup*/
  RX_DATA_INTERRUPT_DRIVEN = 0x62, 
  /**RX triggered by Interrupt. Automatically deactivated at Startup*/
  RX_DATA_INTERRUPT_DRIVEN_INACTIVE = 0x60, 
  /**TX triggered by SW. Will cause an Interrupt on completion*/
  TX_DATA_INTERRUPT_ACTIVE = 0x50, 
  /** MOB is unused*/
  UNUSED = 0x00
}mob_purpose;

/**\brief Settings for a MOB*/
struct mob_settings
{
  
  /** \brief CAN ID
  *  
  * Input for Transmisson.<br>
  * Input or Output for Receiving. Depending on the CAN Mask.<br>
  */
  uint16_t can_id;  
  /** \brief CAN Mask
  
  Mask of the CAN ID for RX.<br>
  Input for Receiving, Irrelevant for Transmitting<br><br>
  
  The Can Message is filtered by a Acceptance filter according to this Mask.<br>
  This Element is bit coded. 0 means this bit in the CAN ID is not relevant for Acceptance. 1 means it is relevant.<br>

  for example:
Configured CAN ID | Configured CAN MSK | Actual CAN ID | ACCEPTANCE
:---------------- | :----------------- | :------------ | :--------:
7E0               | 3FF                | 7E0           | YES
7E0               | 3FF                | 7E8           | NO
7E0               | 3F7                | 7E8           | YES
7E0               | 000                | ANY           | YES

  */
  uint16_t can_msk; 
  /** \brief Extended Message Format   

    this element is set to 1 if the extended Message Format is used.
    Input for Transmitting and Receiving
  */
  uint8_t ide;      
  /** \brief Data Length Coding
   *  
   *  Length of the Data.
   *  Output for Receiving. Input for Transmission
  */
  uint8_t dlc;      
  /** \brief Pointer to the Data array 

    The Data arry has to be already allocated.
  */
  uint8_t *data;    
  
};

/**\brief configuration object for the FRMMan Settings*/
struct MobConfigElement
{
  /**purpose of the MOB*/
  mob_purpose op;       
  /**settings struct*/
  mob_settings ms;      
  /**ISR routine*/
  void (*f)(uint8_t);   
  /**OUT: timestamp of the Last Interrupt*/
  uint32_t timestamp;   
  /**additional Data for usage in an custom ISR*/
  void* additionalData; 
};


/*--------------------- CANDrv Section ---------------------*/
/**
  \fn uint8_t CANDrv_Init(CanBaudrate baudrate)
  \brief Initialize the CAN Driver
  @param baudrate the baudrate to set
  @return 1 on success. 0 in case of an error.
*/
uint8_t CANDrv_Init(CanBaudrate baudrate);
/*--------------------- FRMMan Section ---------------------*/
/**
  \fn CANDrv_FRMMan_Get_Msg(uint8_t index, mob_settings *ms)
  \brief get a received Can Message.
  
  This function reads a received Can Message from the internal Buffer.<br>
  It also retries to read if there were any Changes, because of an Interrupt, receiving a new Message.<br>
  In this case the read operation is retried.<br>
  @param index the index of the mob to read
  @param ms OUTPUT Pointer to a message settings object, where the data is written.
  @return 1 on success. 0 in case of an error.
*/
uint8_t CANDrv_FRMMan_Get_Msg(uint8_t index, mob_settings *ms);
/**
  \fn CANDrv_FRMMan_Send_Msg(uint8_t index)
  \brief sends a configured Message
  
  This function sends a predefined Message.
  @param index the index of the Message to send
  @return 1 on success. 0 in case of an error.
*/
uint8_t CANDrv_FRMMan_Send_Msg(uint8_t index);
/**
  \fn CANDrv_FRMMan_Init(MobConfigElement *CAN_Config);
  \brief Initialize the FRMMan
  
  This function initializes the FRMMan with the given CAN_Config.<br><br>
  Example configuration:
  \code{.cpp}
  //Make sure all these variables are global an not in a scoped context, like a funtion.
  uint8_t data_130[8];
  uint8_t data_7e8[8];
  uint8_t data_7e0[8];
  MobConfigElement CAN_Config[] =
  {
    {TX_DATA_SW_DRIVEN,{0x7e0,0x000,0,8,(uint8_t*)&data_7e0}},//This Messsage can be sent with CANDrv_FRMMan_Send_Msg(0);
    {RX_DATA_INTERRUPT_DRIVEN,{0x130,0x3FF,0,8,(uint8_t*)&data_130},&receiveData_generic_restart,0,0},//This Message will be received with an Interrupt. It will be directly enabled again
    {RX_DATA_INTERRUPT_DRIVEN,{0x7e8,0x3FF,0,8,(uint8_t*)&data_7e8},&receiveData_generic,0,0},//This Message will be received with an Interrupt. It will be only received once!
    {UNUSED},//unused MOBs
    {UNUSED},
    {UNUSED}
  };
  
  void setup() {
  data_7e0[0] = 0x02; //Initialize the Message to send.
  data_7e0[1] = 0x01; 
  data_7e0[2] = 0x05; 
  data_7e0[3] = 0x33;
  data_7e0[4] = 0x44;
  data_7e0[5] = 0x55;
  data_7e0[6] = 0x66;
  data_7e0[7] = 0x77;
  CANDrv_Init(CAN_500k); //init CANDrv
  CANDrv_FRMMan_Init(CAN_Config);//init FRMMan
  sei();//enable interrupts. This is neccessary for the Interrupt driven receives
}
  \endcode
  
  @param CAN_Config The CAN_Config to use. For examples see funtion description.
  @return 1 on success. 0 in case of an error.
*/
uint8_t CANDrv_FRMMan_Init(MobConfigElement *CAN_Config);

/**
  \fn receiveData_generic(uint8_t mob_NR)
  \brief generic Receive funtion for an Interrupt Driven MOB.

  This function only receives this Message once!
  \param mob_NR the MOB Number given by the ISR
*/
void receiveData_generic(uint8_t mob_NR);
/**
  \fn receiveData_generic_restart(uint8_t mob_NR)
  \brief generic Receive funtion for an Interrupt Driven MOB.

  This function will activate the Interrupt again.
  \param mob_NR the MOB Number given by the ISR
*/
void receiveData_generic_restart(uint8_t mob_NR);


/*---------------- CANDrv advanced functions ----------------*/
/**
  \fn CANDrv_ClearAll_MOB(void)
  \brief advanced Function not needed for normal operation

  This function clear all MOB registers. This is used in INI.
*/
void CANDrv_ClearAll_MOB(void);
/**
  \fn setupMOB(mob_settings *ms)
  \brief advanced Function not needed for normal operation

  This function writes the given Settings to the currently selected MOB.
  To select a MOB see CANPAGEHandler
  \see CANPAGEHandler
  \param ms pointer to the MOB settings
*/
void getMOBsetup(mob_settings *ms);
/**
  \fn getMOBsetup(mob_settings *ms)
  \brief advanced Function not needed for normal operation

  This function reads the Settings from the currently selected MOB.
  To select a MOB see CANPAGEHandler
  \see CANPAGEHandler
  \param ms pointer to the MOB settings
*/
void getMOBsetup(mob_settings *ms);


/**\class CANPAGEHandler
*  \brief Class for safe handling of the CANPAGE register.
*
*  This class is initialized with the mob_nr to switch to.
*  The current CANPAGE is saved an will be restored after destruction of the context
*/
class CANPAGEHandler
{
    private:
      uint8_t canpage_tmp;
    public:
    /**\fn CANPAGEHandler(uint8_t mob_nr)
     * \brief set CANPAGE to the supplied NR
     * \param mob_nr the Number of the MOB to switch to
     */
    CANPAGEHandler(uint8_t mob_nr);
    /**\fn ~CANPAGEHandler()
     * \brief recovers the previously saved CANPAGE
     */
    ~CANPAGEHandler();
};
/**
  \fn setMOB_Operation(mob_operation mo)
  \brief set the Operation of the current MOB

  This function sets the Operation of the currently selected MOB
  To select a MOB see CANPAGEHandler
  \see CANPAGEHandler
  \param mo the operation mode to set the MOB to
*/
inline void setMOB_Operation(mob_operation mo)
{
  CANCDMOB &= 0x3F;//clear CONMOB
  CANCDMOB |= (mo & 0x03) << CONMOB0;
  //if(mo == DISABLED)
  //{ uint8_t  volatile *__i_; for (__i_=&CANSTMOB; __i_<&CANSTML; __i_++) { *__i_=0x00 ;}}
}

