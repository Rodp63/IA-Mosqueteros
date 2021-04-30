const numberOfCities = 10;

const numberOfChromosomes = 50;

const numberOfIterations = 50;

let cities = [];           //city = {id:"1", cords: [0.123, 45.1243]}
let roads = [];            //road = {source:"1", target:"2"}, 51
                                
/////////AUXILIAR FUNCTIONS///////

const eucledianDistance = (x1, y1, x2, y2) => {
    return (Math.sqrt(Math.pow(x2-x1,2) + Math.pow(y2-y1,2)));
}

const shuffle = (array) => {
    let currentIndex = array.length, temporaryValue, randomIndex;
    while (0 !== currentIndex) {
      randomIndex = Math.floor(Math.random() * currentIndex);
      currentIndex -= 1;
      temporaryValue = array[currentIndex];
      array[currentIndex] = array[randomIndex];
      array[randomIndex] = temporaryValue;
    }
    return array;
}


/////////SETTING UP DATA GRAPH//////////
const generateCities = () =>{
    for(let i = 0; i < numberOfCities; i++){
        const x = (Math.random() * 200) - 100;
        const y = (Math.random() * 160) - 80;
        cities.push({id: i.toString(), coords: [x, y]});        
    }
}

const linesForMap = [];

const generateRoads = () => {
    const arrayOfRoads = [];
    for(let i = 0; i < numberOfCities; i++){
        for(let j = i+1; j < numberOfCities; j++){
            const road = [];
            road.push({source: cities[i].id, target: cities[j].id});
            road.push(eucledianDistance(cities[i].coords[0], cities[i].coords[1], cities[j].coords[0], cities[j].coords[1]));
            arrayOfRoads.push(road);

            linesForMap.push([[cities[i].coords, cities[j].coords]]);
        }
    }
    return arrayOfRoads;
}


// cities.push({id: "1"});
// cities.push({id: "2"});
// cities.push({id: "3"});
// cities.push({id: "4"});

// roads.push([{source: "1", target: "2"}, 10])
// roads.push([{source: "1", target: "3"}, 15])
// roads.push([{source: "1", target: "4"}, 20])
// roads.push([{source: "2", target: "4"}, 25])
// roads.push([{source: "2", target: "3"}, 30])
// roads.push([{source: "3", target: "4"}, 30])


//GENERATE RANDOM DATA
 generateCities();
 roads = generateRoads();


/////////SETTING UP THE DATA ALGORITHM//////////

const generateSecuence = () => {
    const secuence = [];

    for(let i = 1; i < numberOfCities; i++){
        secuence.push(i.toString());
    }
    return secuence;
}




const generateChromosomes = (numberOfChromosomes) =>{
    const result = [];

    for(let i = 0; i < numberOfChromosomes; i++){
        const secuence = generateSecuence();
        result.push(shuffle(secuence))
    }

    return(result);
}

const crossOver = (parent1, parent2) =>{
    const offspring = [];
    const valuesToRemove = [];

    const parent2Indexes = [];

    for(let i = 0; i < Math.floor(parent1.length/2); i++){
        valuesToRemove.push(parent1[i]);
    }

    for(let i = Math.floor(parent1.length/2); i < parent1.length; i++){
        offspring.push(parent1[i]);
    }

    for(let i = 0; i < valuesToRemove.length; i++){
        parent2Indexes.push(parent2.indexOf(valuesToRemove[i]));
    }


    parent2Indexes.sort((a,b)=>(a - b));

    for(let i = 0; i < parent2Indexes.length; i++){
        offspring.push(parent2[parent2Indexes[i]]);
    }

    return offspring;
}


const mutation = (parent) =>{
    const offspring = [];

    const index1 = Math.floor(parent.length * 0.2);
    const index2 = Math.floor(parent.length * 0.7);

    let arrayToSuffle = [];

    for(let i = index1; i < index2; i++){
        arrayToSuffle.push(parent[i]);
    }

    arrayToSuffle = shuffle(arrayToSuffle);

    for(let i = 0; i < index1; i++){
        offspring.push(parent[i]);
    }

    for(let i = 0; i < arrayToSuffle.length; i++){
        offspring.push(arrayToSuffle[i]);
    }

    for(let i = index2; i < parent.length; i++){
        offspring.push(parent[i]);
    }

    return offspring;
}

const generateChromosomesWithCrossOver = (chromosomes, n) => {
    const newChromosomes = [];
    for(let i = 0; i < n; i++){
        const parent1 = chromosomes[Math.floor(Math.random()*chromosomes.length)];
        const parent2 = chromosomes[Math.floor(Math.random()*chromosomes.length)];
        
        let newChromosome = crossOver(parent1, parent2);
        newChromosomes.push(newChromosome);
    }
    return newChromosomes;
}

const generateChromosomesWithMutation = (chromosomes, n) => {
    const newChromosomes = [];
    for(let i = 0; i < n; i++){
        const parent = chromosomes[Math.floor(Math.random()*chromosomes.length)];
        let newChromosome = mutation(parent);
        newChromosomes.push(newChromosome);
    }
    return newChromosomes;
}

const getChromosomeFit = (chromosome) => {
    let distance = 0;
    for(let i = 0; i < chromosome.length - 1; i++){
        for(let j = 0; j < roads.length; j++){
            if((roads[j][0].source === chromosome[i] && roads[j][0].target === chromosome[i+1]) || (roads[j][0].source === chromosome[i+1] && roads[j][0].target === chromosome[i])){
                distance += roads[j][1];
            }
        }
    }

    const firstNode = "1";
    const secondNode = chromosome[0];
    const lastNode = chromosome[chromosome.length-1];

    for(let j = 0; j < roads.length; j++){
        if((roads[j][0].source === firstNode && roads[j][0].target === secondNode) || (roads[j][0].source === secondNode && roads[j][0].target === firstNode)){
            distance += roads[j][1];
        }
    }

    for(let j = 0; j < roads.length; j++){
        if((roads[j][0].source === firstNode && roads[j][0].target === lastNode) || (roads[j][0].source === lastNode && roads[j][0].target === firstNode)){
            distance += roads[j][1];
        }
    }

    return distance;
}


const findLargestChomosomes = (array, numberOfLargest) => {
    const calculation = array;

    calculation.sort((a, b) => getChromosomeFit(a) > getChromosomeFit(b) ? 1 : getChromosomeFit(a) < getChromosomeFit(b) ? -1 : 0);
    
    return calculation.slice(0, numberOfLargest);
}

const fitness = (chromosomes , numberOfSurvivers) => {
    const survivers = [];


    for(let i = 0; i < chromosomes.length; i++){
        survivers.push(chromosomes[i]);
    }

    const result = findLargestChomosomes(survivers, numberOfSurvivers)

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////ALGORITHM/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const initialChromosomes = generateChromosomes(numberOfChromosomes);

const dataForChart = [];

/****************/
const iterationsProm = [];

const addProm = (chromosomes) => {
    let cum = 0;
    for(let i = 0; i < chromosomes.length; i++){
        cum+=getChromosomeFit(chromosomes[i]);
    }
    cum /= chromosomes.length;
    iterationsProm.push(cum);
}
/****************/

const geneticAlgorithm = (numberOfIterations) => {

    let bestChromosomes = generateChromosomes(numberOfChromosomes);

    for(let j = 0; j < numberOfIterations; j++){

        let crossOverChromosomes = generateChromosomesWithCrossOver(bestChromosomes, 32);

        let mutationChromosomes = generateChromosomesWithMutation(bestChromosomes, 14);

        let newRandomChromosomes = generateChromosomes(4)


        for(let i = 0; i < initialChromosomes.length; i++){
            bestChromosomes.push(initialChromosomes[i]);   
        }

        for(let i = 0; i < crossOverChromosomes.length; i++){
            bestChromosomes.push(crossOverChromosomes[i]);   
        }

        for(let i = 0; i < mutationChromosomes.length; i++){
            bestChromosomes.push(mutationChromosomes[i]);   
        }

        for(let i = 0; i < newRandomChromosomes.length; i++){
            bestChromosomes.push(newRandomChromosomes[i]);   
        }
        addProm(bestChromosomes);

        bestChromosomes = fitness(bestChromosomes, 50);

        dataForChart.push(getChromosomeFit(fitness(bestChromosomes, 1)[0]))
    }

    return fitness(bestChromosomes, 1);
}

const solution = geneticAlgorithm(numberOfIterations);
console.log("SOLUTION" ,solution);

const getCoord = (id) => {
    for(let i = 0; i < cities.length; i++){
        if(cities[i].id === id){
            return cities[i].coords;
        }
    }
}

const formatSolution = (solution) => {
    const shortestRoad = [];
    
    shortestRoad.push([cities[0].coords, getCoord(solution[0][0])]);

    for(let i = 0; i < solution[0].length -1; i++){
        shortestRoad.push([getCoord(solution[0][i]), getCoord(solution[0][i+1])])
    }
    
    shortestRoad.push([ getCoord(solution[0][solution[0].length-1]), cities[0].coords]);

    return shortestRoad;
}




const shortestRoad = formatSolution(solution);

console.log("SHORTEST PATH",shortestRoad);

export {cities, roads, shortestRoad, dataForChart, numberOfCities, linesForMap, numberOfIterations, iterationsProm};