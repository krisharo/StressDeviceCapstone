<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="author" content="Kris Haro">
  <meta name="description" content="Device Sensor Data">
  <title>Tech Demo Web</title>
  <link rel="stylesheet" href="css/styling.css"
</head>
<body>
  <script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.2/Chart.bundle.min.js"></script>
  <script src="path/to/chartjs/dist/chart.min.js"></script>
  <script src="path/to/chartjs-plugin-annotation/dist/chartjs-plugin-annotation.min.js"></script>
  <h1>Sensor Data</h1>
<?php
/*
  Original author Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Modifications done by Kris Haro for ECE44x project
// Will plot the sql data in different graphs

$dbhost = 'classmysql.engr.oregonstate.edu';
$dbname = 'capstone_2021_harokr';
$dbuser = 'capstone_2021_harokr';
$dbpass = 'VxbYGMg1qJi!h.2Z';

// Create connection
$conn = new mysqli($dbhost, $dbuser, $dbpass, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
// get stuff from table
$sql = "SELECT id, bTemp, hr, gsr, switch, timeS FROM StressDeviceEntry ORDER BY id ASC";

if ($result = $conn->query($sql)) { // execute query
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_bTemp = $row["bTemp"];
        $row_heartRate = $row["hr"];
        $row_skinResponse = $row["gsr"];
        $row_switch = $row["switch"];
        $row_timeS = $row["timeS"];

        $data1 = $data1 . '"'. $row['bTemp']. '",'; // store query values into arrays
        $dataH = $dataH . ''. $row['hr']. ',';
        $dataG = $dataG . '"'. $row['gsr']. '",';
        $dataR = $dataR . '"'. $row['switch']. '",';
        $dataT = $dataT . '"'. $row['timeS']. '",';
    }
    $data1 = trim($data1,",");
    $dataH = trim($dataH,",");
    $dataG = trim($dataG,",");
    $dataR = trim($dataR,",");
    $dataT = trim($dataT,",");
    $result->free(); // free memory associated with result
}

$conn->close(); // close sql connection
?>
</table>
<!-- create graphs -->
<h2>Relative Temperature over Time </h2>
<div class="container">
  <canvas id="tempChart">
  </canvas>
  <script>
  // javascript code for one graph, using chart.js library
  var ctx = document.getElementById("tempChart").getContext('2d');
    var myChart = new Chart(ctx, {
      type: 'line',
      data: {
          labels:[<?php echo $dataT; ?>],
          datasets:
          [{
              label: 'Body Temp',
              data: [<?php echo $data1; ?>],
              backgroundColor: 'transparent',
              borderColor:'rgba(255,99,132)',
              borderWidth: 3
          }]
      },

      options: {
        responsive: true,
        maintainAspectRatio: true,
        scales: {scales:{yAxes: [{beginAtZero: false}], xAxes: [{autoskip: true, maxTicketsLimit: 20}]}},
        tooltips:{mode: 'index'},
        legend:{display: true, position: 'top', labels: {fontColor: 'rgb(0,0,0)', fontSize: 16}}
      } // options

  });
  </script>
</div>
<!-- another graph -->
<h2>Heart Rate </h2>
<div class="container">
  <canvas id="hrChart">
    <script>
    var ctx = document.getElementById("hrChart").getContext('2d');
    const average = 3;
    const hr = [<?php echo $dataH; ?>];
    const time = [<?php echo $dataT; ?>];
    const movingAverage = [];
    for(i = 0; i < hr.length -2; i++){
      const threedatapoints = hr.slice(i, average + i);
      threedatapoints.reduce((total, num) => total + num) / 3;
      movingAverage.push(threedatapoints.reduce((total, num) => total + num) / threedatapoints.length);
      console.log(movingAverage)
    }
    var myChart = new Chart(ctx, {
      type: 'line',
      data: {
          labels: time,
          datasets:
          [{
            label: 'Heart Rate',
            data: hr,
            backgroundColor: 'transparent',
            borderColor:'rgba(100,200,255)',
            borderWidth: 3
          },{
            label: 'Moving Average',
            backgroundColor: 'transparent',
            borderColor: 'rgb(0,99,0)',
            data:movingAverage
          }]
      },

      options: {
        responsive: true,
        maintainAspectRatio: true,
        scales: {scales:{yAxes: [{beginAtZero: false}], xAxes: [{autoskip: true, maxTicketsLimit: 20}]}},
        tooltips:{mode: 'index'},
        legend:{display: true, position: 'top', labels: {fontColor: 'rgb(0,0,0)', fontSize: 16}}
      } // options
    }); // myChart
    </script>

  </canvas>
</div>

<h2>Galvanic Skin Respone</h2>
<div class="container">
  <canvas id="gsrChart">
    <script>
    var ctx = document.getElementById("gsrChart").getContext('2d');
      var myChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels:[<?php echo $dataT; ?>],
            datasets:
            [{
              label: 'Skin Response',
              data: [<?php echo $dataG; ?>],
              backgroundColor: 'transparent',
              borderColor:'rgba(100,100,230)',
              borderWidth: 3
            }]
        },

        options: {
          responsive: true,
          maintainAspectRatio: true,
          scales: {scales:{yAxes: [{beginAtZero: false}], xAxes: [{autoskip: true, maxTicketsLimit: 20}]}},
          tooltips:{mode: 'index'},
          legend:{display: true, position: 'top', labels: {fontColor: 'rgb(0,0,0)', fontSize: 16}}
        } // options
    });
    </script>
  </canvas>
</div>
<button id="new entry"> Create new entry </button>
<button id="export"> Export data </button>
</body>
<script src="js/dataPage.js"></script>
<script type="text/javascript">
    // assume each element of $arrayWithVars has already been json_encoded
    toCSV([<?php echo $data1 ?>], [<?php echo $data2 ?>], [<?php echo $dataH ?>], [<?php echo $dataG ?>], [<?php echo $dataR ?>], [<?php echo $dataT ?>]);
</script>

</html>
