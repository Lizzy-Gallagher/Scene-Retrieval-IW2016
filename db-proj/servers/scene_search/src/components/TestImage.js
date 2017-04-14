import React, { Component } from 'react';

import 'whatwg-fetch'

class TestImage extends Component {
  constructor() {
    super();
    this.data = []

    this.fetchResults = this.fetchResults.bind(this);
  }

  fetchResults(url) {
    var self = this;
    fetch(url).then(function(response) {
        return response.json()
      }).then(function(json) {

        self.data = json.test_results;
        console.log(json);

      }).catch(function(ex) {
        console.log('Bug in TestImage.fetchResults', ex)
       })
    }
  
  createImageCall(hash,objectList) {
    let baseURL = 'http://localhost:2000/getimage?'
    baseURL += 'hash=' + hash
    baseURL += '&';
    baseURL += 'objects=' + objectList.join(',')
    return baseURL;
  }

  render() {
    //this.fetchResults('http://localhost:5000/test?object=sofa')
    let testFilename = this.createImageCall('0020d9dab70c6c8cfc0564c139c82dce', ['0_22','0_64','0_39'])

    return (
      <div className="container-fluid">
        <h1>TestImage</h1>
        <img id="image" src={testFilename} />
      </div>
    );
  }
}

export default TestImage;
