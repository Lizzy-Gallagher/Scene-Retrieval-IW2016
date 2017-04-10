import React, { Component } from 'react';
import 'whatwg-fetch'
import $ from 'jquery'

import QueryBox from './QueryBox';
import ResultList from './ResultList';
import SearchOptions from './SearchOptions';
import DatabaseStats from './DatabaseStats';
import IconAttribution from './IconAttribution';

class Main extends Component {
  constructor() {
    super();
    this.state = {
      query: '',
      returnType: 'scene',
      databaseURL: 'http://localhost:5000/',
      nlpURL: 'http://localhost:4000/',
      sceneData: [],
      levelData: [],
      roomData:  [],
      showNotFoundError: false
    };

    this.tempSceneData = [];
    this.tempLevelData = [];
    this.tempRoomData = [];

    this.handleChange = this.handleChange.bind(this);
    this.handleClick  = this.handleClick.bind(this);
    this.handleSelectChange = this.handleSelectChange.bind(this);
    this.queryDatabase = this.queryDatabase.bind(this);
    this.fetchResultsFromServer = this.fetchResultsFromServer.bind(this);
  }

  handleChange(text) {
    this.setState({query: text})
  }

  handleClick() {
    this.fetchResultsFromServer();
  }

  handleSelectChange(event) {
    this.setState({returnType: event.target.value});
  }

  queryDatabase(apiCall, params, toInclude) {
    let self = this;
    return $.Deferred(function() {
      let defer = this;
      let url = self.state.databaseURL + apiCall;

      // Format the api call's url
      let esc = encodeURIComponent;

      url = url + '?' + Object.keys(params)
          .map(key => esc(key) + '=' + esc(params[key]))
          .join('&');

      console.log('url:', url);
      fetch(url).then(function(response) {
        // Format as json
        return response.json()
      }).then(function(json) {
        
        // If no results, show not found error
        self.setState({
           showNotFoundError : json.scene_results.length == 0
        });

        self.tempSceneData.push({ results: Main.score(json.scene_results), toInclude: toInclude});
        self.tempLevelData.push({ results: Main.score(json.level_results), toInclude: toInclude});
        self.tempRoomData.push({ results: Main.score(json.room_results), toInclude: toInclude});

        // Required to return this "deferred" function
        defer.resolve();
      })
      .catch(function(ex) {
        console.log('parsing failed', ex)
        defer.resolve();
       })
    })
  }

  static sortByValue(arr) {
    return arr.sort(function(a,b) {
      return b.value - a.value
    })
  }

  static intersect(currentData, tempData) {
    return currentData.filter(function(element) {
        return tempData.some(function(e) {
          if (e.scene_hash == element.scene_hash)
            element.value += e.value

          return e.scene_hash == element.scene_hash
        })
    })
  }

  static difference(currentData, tempData) {
    return currentData.filter(function(element) {
      let exists = tempData.some(function(e) {
        return e.scene_hash == element.scene_hash
      })
      return !exists
    })
  }

  static filter(tempData) {
    var currentData = tempData[0].results;
    for (var i = 1; i < tempData.length; i++) {
      if (tempData[i].toInclude > 0)
        currentData = Main.intersect(currentData, tempData[i].results)
      else {
        currentData = Main.difference(currentData, tempData[i].results)
      }
    }
    return currentData;
  }

  static maximum(results) {
    var max = -1;
    for (var i = 0; i < results.length; i++) {
      if (results[i].value > max)
        max = results[i].value
    }
    return max
  }

  static minimum(results) {
    var min = Infinity;
    for (var i = 0; i < results.length; i++) {
      if (results[i].value < min)
        min = results[i].value
    }
    return min
  }

  static score(tempData) {
    var max = Main.maximum(tempData)
    var min = Main.minimum(tempData)
    tempData = tempData.map(function(e){
      e.value = 100 * ((e.value - min) / (max - min))
      return e
    })

    return tempData
  }

  fetchResultsFromServer() {
    let self = this;

    // Reset temporary data
    self.tempSceneData = []
    self.tempLevelData = []
    self.tempRoomData = []

    // First get the appropriate api call from other server
    let queryParams = this.state.query.split(' ').join('+');
    fetch(this.state.nlpURL + 'api?query='+queryParams)
      .then(function(response) {
        return response.json()
      }).then(function(json) {
        console.log(json.apiCalls)
        // Make calls
        var requests = Array();
        json.apiCalls.forEach(function(apiCall) {
          requests.push(self.queryDatabase(apiCall.apiCall, apiCall.params, apiCall.toInclude));
        })

        var defer = $.when.apply($, requests);
        defer.done(function() {
          // Filter results (TODO: Just binaries)
          var filteredSceneData = Main.filter(self.tempSceneData)
          var filteredLevelData = Main.filter(self.tempLevelData)
          var filteredRoomData = Main.filter(self.tempRoomData)
          
          // Update state
          self.setState({
            sceneData : Main.sortByValue(filteredSceneData),
            levelData : Main.sortByValue(filteredLevelData),
            roomData  : Main.sortByValue(filteredRoomData),
          })
        })
      })
  }


  render() {
    return (
      <div className="container-fluid">
        <h1>SUNCG Scene Search</h1>
        <DatabaseStats databaseURL={ this.state.databaseURL } />
        <SearchOptions handleSelectChange={ this.handleSelectChange } />
        <QueryBox databaseURL={ this.state.databaseURL }
                  query={ this.state.query }
                  handleChange={ this.handleChange }
                  handleClick={() => this.handleClick()} />
        <ResultList sceneData = { this.state.sceneData }
                    levelData = { this.state.levelData }
                    roomData  = { this.state.roomData }
                    returnType = { this.state.returnType }
                    showNotFoundError = { this.state.showNotFoundError }/>
        <IconAttribution />
      </div>
    );
  }
}

Main.defaultProps = {
};

export default Main;
