import './App.css';

import { Graph } from "react-d3-graph";
import  MyChart from './chart.component';

import {cities, roads, shortestRoad, dataForChart} from './utils';

const App = () => {

  const formatRoads = (roads)=>{
    const newRoads = [];
    for(let i = 0; i < roads.length; i++){
      newRoads.push(roads[i][0]);
    }
    return newRoads;
  }

  const newRoads = formatRoads(roads);

  const data = {
    nodes: cities,
    links: newRoads
  };

  const dataShortestPath = {
    nodes: cities,
    links: shortestRoad
  }

  

  const onClickNode = function(nodeId) {
    window.alert(`Clicked node ${nodeId}`);
  };
  
  const onClickLink = function(source, target) {
    window.alert(`Clicked link between ${source} and ${target}`);
  };
  

  const myConfig = {
    nodeHighlightBehavior: true,
    node: {
      color: "lightgreen",
      size: 120,
      highlightStrokeColor: "blue",
    },
    link: {
      highlightColor: "lightblue",
    },
  };
  
  const myConfigSolution = {
    nodeHighlightBehavior: true,
    directed:true,
    node: {
      color: "lightgreen",
      size: 120,
      highlightStrokeColor: "blue",
    },
    link: {
      highlightColor: "lightblue",
    },
  };



  /////////////////////////////////////////////////////////

  
  return (
    <div className="App">
      <Graph
        id="graph-id" // id is mandatory
        data={data}
        config={myConfig}
        onClickNode={onClickNode}
        onClickLink={onClickLink}
      />
      <Graph
        id="solution" // id is mandatory
        data={dataShortestPath}
        config={myConfigSolution}
        onClickNode={onClickNode}
        onClickLink={onClickLink}
      />;
      <MyChart dataForChart={dataForChart}/>
    </div>
  );
}

export default App;
