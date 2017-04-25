/**
 * Created by erb2 on 2/8/17.
 */

import React, { Component } from 'react';

import 'whatwg-fetch'

class DatabaseStats extends Component {
  constructor(props) {
    super(props);
  }

  loadCountFromServer() {
    fetch(this.props.databaseURL + 'numScenes')
      .then(function(response) {
        return response.json();
      }).then(function(json) {
          document.getElementById('numScenes').innerHTML += json.numScenes.toLocaleString() + ' scenes';
      });
  }

  componentDidMount() {
    this.loadCountFromServer();
  }

  render() {
    return (
        <small id="numScenes"><span style={{display: 'inline-block', width: '25px'}}></span> </small>
    );
  }
}

export default DatabaseStats;
