import React from "react";
import {
  ComposableMap,
  Geographies,
  Geography,
  Marker,
  Line
} from "react-simple-maps";

import {cities, numberOfCities, linesForMap, shortestRoad} from "./utils";

const geoUrl =
  "https://raw.githubusercontent.com/zcreativelabs/react-simple-maps/master/topojson-maps/world-110m.json";

const markers = [
];
console.log(shortestRoad);
const lines = linesForMap;


for(let i = 0; i < numberOfCities; i++){
    markers.push({markerOffset: 20, name: cities[i].id, coordinates: cities[i].coords})
}

const MapChart = () => {
  return (
    <ComposableMap
      projection="geoAzimuthalEqualArea"
      projectionConfig={{
        rotate: [58, 20, 0],
      }}
    >
      <Geographies geography={geoUrl}>
        {({ geographies }) =>
          geographies
            .filter(d => d.properties.REGION_UN === "")
            .map(geo => (
              <Geography
                key={geo.rsmKey}
                geography={geo}
                fill="#EAEAEC"
                stroke="#D6D6DA"
              />
            ))
        }
      </Geographies>
      {markers.map(({ name, coordinates, markerOffset }) => (
        <Marker key={name} coordinates={coordinates}>
          <circle r={5} fill="#F00" stroke="#fff" strokeWidth={2} />
          <text
            textAnchor="middle"
            y={markerOffset}
            style={{ fontFamily: "system-ui", fill: "#5D5A6D" }}
          >
            {name}
          </text>
        </Marker>
      ))}
      {
          lines.map(line =>
            <Line
                from={line[0][0]}
                to={line[0][1]}
                stroke="#000000"
                strokeWidth={1}
                strokeLinecap="round"
            />
          )
      }
      
      {
          shortestRoad.map(road =>{
              console.log(road);
              return(
                <Line
                    from={road[0]}
                    to={road[1]}
                    stroke="#00008B"
                    strokeWidth={1}
                    strokeLinecap="round"
                />
              )
          }
          )
      }
      <Line
        from={shortestRoad[shortestRoad.length - 1][0]}
        to={shortestRoad[shortestRoad.length - 1][1]}
        stroke="#006400"
        strokeWidth={1}
        strokeLinecap="round"
      />
    </ComposableMap>
  );
};

export default MapChart;