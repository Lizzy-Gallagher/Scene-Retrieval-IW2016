/**
 * Created by erb2 on 2/8/17.
 */

import React, { Component } from 'react';

class SearchOptions extends Component {
  render() {
    return (
      <div className="form-inline">
        <label className="mr-sm-2" htmlFor="select">View by: </label>
        <select onChange={ this.props.handleSelectChange } className="form-control mb-2 mr-sm-2 mb-sm-0 col-1" id="searchOption">
          <option value="scene">Scene</option>
          <option value="level">Level</option>
          <option value="room">Room</option>
        </select>
      </div>
    );
  }
}

export default SearchOptions;