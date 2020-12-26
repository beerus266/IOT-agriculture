/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 * @flow strict-local
 */

import React, { useEffect, useState } from 'react';
import {
  SafeAreaView,
  StyleSheet,
  ScrollView,
  View,
  Text,
  StatusBar,
  ClippingRectangle,
} from 'react-native';

import {
  Header,
  LearnMoreLinks,
  Colors,
  DebugInstructions,
  ReloadInstructions,
} from 'react-native/Libraries/NewAppScreen';

import MQTT from 'sp-react-native-mqtt';
import firebase from 'firebase';
import SwitchButton from './Components/SwitchButton'

var firebaseConfig = {
  apiKey: "AIzaSyC3lng4k8mGKTY5ysTq6Su2MfR5-tgAkSA",
  authDomain: "iot-smartagriculture-project3.firebaseapp.com",
  databaseURL: "https://iot-smartagriculture-project3-default-rtdb.firebaseio.com",
  projectId: "iot-smartagriculture-project3",
  storageBucket: "iot-smartagriculture-project3.appspot.com",
  messagingSenderId: "952109118404",
  appId: "1:952109118404:web:a550534cea3af57b3daa91",
  measurementId: "G-8CVS1DV85J"
};

var globalClient;

const App: () => React$Node = () => {

   const [temp,setTemp]=useState();
   const [humi,setHumi]=useState();
   const [humiOS,setHumiOS]=useState();



  useEffect( () => {

    // =======MQTT Broker==========
    MQTT.createClient({
      uri: 'mqtt://broker.hivemq.com:1883',
      clientId: 'c1152957-3dbc-4de1-8575-16b83f56dec3'
    }).then(function(client) {

      globalClient = client;
     
      client.on('closed', function() {
        console.log('mqtt.event.closed');
      });
     
      client.on('error', function(msg) {
        console.log('mqtt.event.error', msg);
      });
     
      client.on('message', function(msg) {
        console.log('mqtt.event.message', msg);
      });
     
      client.on('connect', function() {
        console.log('connected');
        client.subscribe('IOTHaiTayTuuTopic', 0);
        // client.publish('testHaiTopic', "test", 0, false);
      });
     
      client.connect();
    }).catch(function(err){
      console.log(err);
    });
    //===================================================================================

    // =======FireBase=============

   

    if (!firebase.apps.length) {
      console.log("Init");
      firebase.initializeApp(firebaseConfig);
    }else {
      console.log("App()");
      firebase.app(); // if already initialized, use that one
    }

    database = firebase.database();
    database.ref('Data').limitToLast(1).once('value', (data) => {
      // console.log("Once");
      // console.log(data.val());
      let dbs=data.val();
      let realData= dbs[Object.keys(dbs)[0]];
      // console.log(x);
      // console.log(data.val()[0]["Humi"]);
      setHumi(realData["Humi"]);
      setHumiOS(realData["HumiOS"]);
      setTemp(realData["Temp"]);
    })

    // .then( (snapshot) => {
    //   console.log("snapshot");
    //   console.log(snapshot);
    // });

        // Get data when it have changed
    // database.ref('Data').limitToLast(1).on('value', (data) => {
    //       console.log("change");
    //       console.log(data.toJSON());
    //       // console.log(data.val());
    // });
    //===================================================================================

    getDataFireBaseChange();
  },[]);

  const getDataFireBaseChange = () => {
            // Get data when it have changed
    firebase.database().ref('Data').limitToLast(1).on('value', (data) => {
              console.log("change123123");
              // console.log(data.toJSON());
              let dbs=data.val();
              let realData1= dbs[Object.keys(dbs)[0]];
              setHumi(realData1["Humi"]);
              setHumiOS(realData1["HumiOS"]);
              setTemp(realData1["Temp"]);
        });
  }

  const changeStatusWaterMachine = (nameMachine, status) => {
    
    if(nameMachine == "WaterPump"){

      globalClient.publish('IOTHaiTayTuuTopic', "pump="+status, 0, false);

      firebase.database().ref('MachineStatus').update({
        WaterPump: status,
      });
    } else if (nameMachine == "WaterSpray") {

      globalClient.publish('IOTHaiTayTuuTopic', "spray="+status, 0, false);

      firebase.database().ref('MachineStatus').update({
        WaterSpray: status,
      });
    }
  };

  return (
    <View style={{flex: 1, marginHorizontal: 30}}>
    <Text
      style={{
        textAlign: 'center',
        fontWeight: 'bold',
        fontSize: 40,
        margin: 30,
      }}>
      Smart Garden
    </Text>
    <View style={{flex: 1, flexDirection: 'row'}}>
    <Text style={{
            fontSize: 30,
          }}>Temperature :   {temp}
    </Text>
    </View>
    <View style={{flex: 1, flexDirection: 'row'}}>
    <Text style={{
            fontSize: 30,
          }}>Humidity of Air :  {humi}
    </Text>
    </View>
    <View style={{flex: 1, flexDirection: 'row'}}>
    <Text style={{
            fontSize: 30,
          }}>Humidity of Ground :  {humiOS}
    </Text>
    </View>
    <View style={{flex: 1, flexDirection: 'row'}}>
      <Text
        style={{
          marginRight: 30,
          fontSize: 30,
        }}>
        Water Spray
      </Text>
      <SwitchButton onValueChange={ (val) => {
        console.log("change switch");
        if (val == 1) {
          changeStatusWaterMachine("WaterSpray", 1);
        } else {
          changeStatusWaterMachine("WaterSpray", 2);
        }
      }}>
        </SwitchButton>
      </View>
      <View style={{flex: 1, flexDirection: 'row'}}>
      <Text
        style={{
          marginRight: 30,
          fontSize: 30,
        }}>
        Water Pump
      </Text>
      <SwitchButton onValueChange={ (val) => {
        if (val == 1) {
          changeStatusWaterMachine("WaterPump", 1);
        } else {
          changeStatusWaterMachine("WaterPump", 2);
        }
      }}>

      </SwitchButton>
      </View>
    </View>
  );

};

const styles = StyleSheet.create({
  scrollView: {
    backgroundColor: Colors.lighter,
  },
  engine: {
    position: 'absolute',
    right: 0,
  },
  body: {
    backgroundColor: Colors.white,
  },
  sectionContainer: {
    marginTop: 32,
    paddingHorizontal: 24,
  },
  sectionTitle: {
    fontSize: 24,
    fontWeight: '600',
    color: Colors.black,
  },
  sectionDescription: {
    marginTop: 8,
    fontSize: 18,
    fontWeight: '400',
    color: Colors.dark,
  },
  highlight: {
    fontWeight: '700',
  },
  footer: {
    color: Colors.dark,
    fontSize: 12,
    fontWeight: '600',
    padding: 4,
    paddingRight: 12,
    textAlign: 'right',
  },
});

export default App;
