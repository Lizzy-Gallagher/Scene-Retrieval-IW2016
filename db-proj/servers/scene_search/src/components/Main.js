import React, { Component } from 'react';

import 'whatwg-fetch'
import $ from 'jquery'

import {PageHeader} from 'react-bootstrap'

import QueryBox from './QueryBox';
import ResultList from './ResultList';
import DatabaseStats from './DatabaseStats';
import StatsBar from './StatsBar';

import AdminPanel from './modals/AdminPanel';
import InfoPanel from './modals/InfoPanel';
import TestServerPanel from './modals/TestServerPanel';

import { filter } from '../actions/filter';
import { score } from '../actions/score'

class Main extends Component {
  constructor() {
    super();
    this.state = {
      databaseURL: 'http://localhost:5000/',
      nlpURL: 'http://localhost:4000/',
      imgURL: 'http://localhost:2000/',

      query: '',
      returnType: 'scene',
      sceneData: [],
      levelData: [],
      roomData:  [],

      showNotFoundError: false,
      showLoading: false,
      isVisPossible: false,
    
      doEnableAutosuggest: true,
      doEnableVis: true
    };

    this.tempSceneData = [];
    this.tempLevelData = [];
    this.tempRoomData = [];
    this.isVisPossible = false;

    this.toggleAutosuggest = this.toggleAutosuggest.bind(this);
    this.toggleVis = this.toggleVis.bind(this);
    this.handleChange = this.handleChange.bind(this);
    this.handleSelectChange = this.handleSelectChange.bind(this);
    this.queryDatabase = this.queryDatabase.bind(this);
    this.fetchResultsFromServer = this.fetchResultsFromServer.bind(this);
  }

  toggleAutosuggest() {
    this.setState({ doEnableAutosuggest: !this.state.doEnableAutosuggest})
  }

  toggleVis() {
    this.setState({ doEnableVis: !this.state.doEnableVis })
  }

  handleChange(text) {
    this.setState({query: text})
  }

  handleSelectChange(event) {
    this.setState({returnType: event.target.value});
  }

  // Makes apiCall to the MySQL Database
  queryDatabase(apiCall, params, toInclude) {
    let self = this;
    return $.Deferred(function() {
      let defer = this;
      
      // Format the api call's url
      let url = self.state.databaseURL + apiCall;
      let esc = encodeURIComponent;
      url = url + '?' + Object.keys(params)
          .map(key => esc(key) + '=' + esc(params[key]))
          .join('&');

      console.log('DEBUG url:', url);
      fetch(url).then(function(response) {
        return response.json()
      }).then(function(json) {
        // Add data to temp collection
        self.tempSceneData.push({ results: score(json.scene_results), toInclude: toInclude});
        self.tempLevelData.push({ results: score(json.level_results), toInclude: toInclude});
        self.tempRoomData.push({ results: score(json.room_results), toInclude: toInclude});
        self.isVisPossible = self.isVisPossible || typeof json.enhanced_vis !== 'undefined';

        // Return from "deferred" function
        defer.resolve();
      }).catch(function(ex) {
        console.log('Bug in Main.queryDatabase', ex)
        defer.resolve();
       })
    })
  }

  fetchResultsFromServer() {
    this.setState({
      showLoading: true,
      showNotFoundError: false,
      isVisPossible: false,
      sceneData: [],
      levelData: [],
      roomData: []
    })

    // Reset temporary data
    this.tempSceneData = []
    this.tempLevelData = []
    this.tempRoomData = []
    this.isVisPossible = false;

    let self = this;
    // First get the appropriate api call from other server
    let queryParams = this.state.query.split(' ').join('+');
    fetch(this.state.nlpURL + 'api?query='+queryParams)
      .then(function(response) {
        return response.json()
      }).then(function(json) {
        // If no results, show not found error
        if (typeof json.error !== 'undefined') {
          self.setState({ showNotFoundError: true, showLoading: false});
          return
        }
          
        console.log(json.apiCalls)
        
        // Make database requests
        var requests = Array();
        json.apiCalls.forEach(function(apiCall) {
          requests.push(self.queryDatabase(apiCall.apiCall, apiCall.params, apiCall.toInclude));
        })
        var defer = $.when.apply($, requests);

        // Wait on database requests
        defer.done(function() {
          console.log('defer done')
          // Filter results (TODO: Just binaries)
          var filteredSceneData = filter(self.tempSceneData)
          var filteredLevelData = filter(self.tempLevelData)
          var filteredRoomData = filter(self.tempRoomData)
          
          let showNotFoundError = filteredSceneData.length === 0
          let isVisPossible = self.isVisPossible && self.state.doEnableVis;

          self.setState({
            sceneData : filteredSceneData,
            levelData : filteredLevelData,
            roomData  : filteredRoomData,
            showNotFoundError : showNotFoundError,
            showLoading : false,
            isVisPossible : isVisPossible
          })
        })
      })
  }

  renderLoader() {
    var Loader = require('react-loaders').Loader;
    if (!this.state.showLoading)
       return <div></div>
    return (
      <div className="container"
          style={{
            height: '70vh',
            display: 'flex',
            justifyContent: 'center',
            alignItems: 'center'
           }}
      >
        <Loader type="ball-grid-pulse" active />
      </div>
    )
  }

  render() {
    return (
      <div className="container-fluid">
        <AdminPanel doEnableAutosuggest={ this.state.doEnableAutosuggest }
                    toggleAutosuggest={ this.toggleAutosuggest }
                    doEnableVis={ this.state.doEnableVis }
                    toggleVis={ this.toggleVis }/>
        <InfoPanel />
        <TestServerPanel databaseURL={ this.state.databaseURL }
                         nlpURL={ this.state.nlpURL }
                         imgURL={ this.state.imgURL }/>
        <PageHeader>SUNCG Scene Search <DatabaseStats databaseURL={ this.state.databaseURL } /> </PageHeader>
        <QueryBox query={ this.state.query }
                  handleChange={ this.handleChange }
                  handleClick={() => this.fetchResultsFromServer() }
                  doEnableAutosuggest={ this.state.doEnableAutosuggest }
        />
        <StatsBar numSceneResults={ this.state.sceneData.length }
                  numLevelResults={ this.state.levelData.length }
                  numRoomResults={ this.state.roomData.length }
                  handleSelectChange={ this.handleSelectChange }
        />
        { this.renderLoader() }
        <ResultList sceneData = { this.state.sceneData }
                    levelData = { this.state.levelData }
                    roomData  = { this.state.roomData }
                    returnType = { this.state.returnType }
                    showNotFoundError = { this.state.showNotFoundError }
                    imgURL={this.state.imgURL}
                    doEnableVis={this.state.isVisPossible}/>
      </div>
    );
  }
}

export default Main;
