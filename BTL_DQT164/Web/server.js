const mysql = require('mysql2');
const mqtt = require('mqtt');

const mqttClient = mqtt.connect('mqtt://172.20.10.3:1883');
let lstate = "ON";
let ledC = 0;
const connection = mysql.createConnection({
  host: 'localhost',
  user: 'root', 
  password: '123456789',
  database: 'iot' 
});

connection.connect((err) => {
  if (err) {
    console.error('Lỗi kết nối:', err);
    return;
  }
  console.log('Đã kết nối đến MySQL');
});

const express = require('express');
const app = express();

app.use((req, res, next) => {
  res.header('Access-Control-Allow-Origin', '*');
  next();
});

app.get('/getHistory', (req, res) => {
  const sql = 'SELECT * FROM iot.history ORDER BY id DESC;';
  connection.query(sql, (err, results) => {
    if (err) throw err;
    ledC = results[0].ledc;
    res.json(results);
  });
});

app.get('/getSensor', (req, res) => {
  const sql = 'SELECT * FROM iot.sensor ORDER BY id DESC;';
  connection.query(sql, (err, results) => {
    if (err) throw err;
    res.json(results);
  });
});

app.get('/control', (req, res) => {
  const command = req.query.command;

  if (command === '1') {
      mqttClient.publish('led', 'ON');
      lstate = 'ON';
      ledC += 1;
  } else if (command === '0') {
      mqttClient.publish('led', 'OFF');
      lstate = 'OFF';
      ledC += 1;
  } 

  res.send(`Command sent: ${command}`);
});

mqttClient.on('connect', () => {
  console.log('Kết nối đến MQTT broker');
  mqttClient.subscribe('sensor');
  mqttClient.subscribe('led');
});

mqttClient.on('message', (topic, message) => {
  current = new Date();
  time = current.toLocaleString();
if (topic == 'led') {

    lstate = message.toString();
    const sql = 'INSERT INTO history (led, time, ledc) VALUES (?, ?, ?)';
    const value = [lstate, time, ledC];
    connection.query(sql, value,(err, results) => {
      if (err) throw err;
    });

  } else if (topic == 'sensor'){

    const jsonData = JSON.parse(message.toString());

    const light = jsonData.light;
    const sql = 'INSERT INTO sensor (light, time) VALUES (?, ?)';
    const value = [light, time];
    connection.query(sql, value,(err, results) => {
      if (err) throw err;
    });
  }
});


app.listen(3000, () => {
  console.log('Server đang lắng nghe tại cổng 3000');
});
