import './App.css';

import MyChart from './chart.component';
import MapChart from './MapChart.component';
import MapChartSolution from './MapChartSolution.component';

import {dataForChart, numberOfCities, numberOfIterations, iterationsProm} from './utils';

const App = () => {
  return (
    <div className="App">
      <span>DATOS</span>
      <span>Número de Nodos: {numberOfCities}</span>
      <span>Número de Iteraciones: {numberOfIterations}</span>
      <br></br>
      <br></br>
      <span>GRÁFICA</span>
      <br></br>
      <br></br>
      <div className="solution">
        <MyChart  dataForChart={dataForChart} iterationsProm={iterationsProm}/>
      </div>
      <br></br>
      <br></br>
      <br></br>
      <br></br>

      <span>GRAFO GENERADO</span>
      <div className="solution">
        <MapChart />
      </div>
      <span>SOLUCIÓN GENERADA</span>
      <div className="solution">
        <MapChartSolution />
      </div>
    </div>
  );
}

export default App;
