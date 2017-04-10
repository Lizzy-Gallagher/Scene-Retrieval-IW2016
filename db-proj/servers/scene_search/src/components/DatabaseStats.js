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
          document.getElementById('numScenes').innerHTML += json.numScenes.toLocaleString();
      });
  }

  componentDidMount() {
    this.loadCountFromServer();
  }

  render() {
    return (
        <p id="numScenes">Number of scenes in database: </p>
    );
  }
}

DatabaseStats.defaultProps = {};

export default DatabaseStats;

