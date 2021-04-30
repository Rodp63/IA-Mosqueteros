import './App.css';

import MyChart from './chart.component';
import MapChart from './MapChart.component';
import MapChartSolution from './MapChartSolution.component';

import {dataForChart, numberOfCities, numberOfIterations} from './utils';

const App = () => {
  return (
    <div className="App">
      <span>DATOS</span>
      <span>Numero de Nodos: {numberOfCities}</span>
      <span>Numero de Iteraciones: {numberOfIterations}</span>
      <br></br>
      <br></br>
      <span>GRAFICA</span>
      <br></br>
      <br></br>
      <div className="solution">
        <MyChart  dataForChart={dataForChart}/>
      </div>
      <br></br>
      <br></br>
      <br></br>
      <br></br>

      <span>GRAFO GENERADO</span>
      <div className="solution">
        <MapChart />
      </div>
      <span>SOLUCION GENERADA</span>
      <div className="solution">
        <MapChartSolution />
      </div>
    </div>
  );
}

export default App;
