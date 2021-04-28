import React from 'react'
import { Chart } from 'react-charts'
 
function MyChart(props) {
  const goChart = [];
  for(let i = 0; i < props.dataForChart.length; i++){
    const currentData = [];  
    currentData.push(i);
    currentData.push(props.dataForChart[i]);
    goChart.push(currentData);
  }
  const data = React.useMemo(
    () => [
      {
        label: 'Iterations',
        data: goChart
      }
    ],
    []
  )
 
  const axes = React.useMemo(
    () => [
      { primary: true, type: 'linear', position: 'bottom' },
      { type: 'linear', position: 'left' }
    ],
    []
  )
 return(
    // A react-chart hyper-responsively and continuously fills the available
    // space of its parent element automatically
    <div
      style={{
        width: '400px',
        height: '300px'
      }}
    >
      <Chart data={data} axes={axes} />
    </div>
  )
}

export default MyChart;