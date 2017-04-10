/**
 * Created by erb2 on 2/8/17.
 */

import React, { Component } from 'react';

let searchImg = require('../images/search.svg');

class SearchButton extends Component {
  render() {
    return (
      <span className="input-group-btn">
        <button id="btnQuery" className="btn btn-danger" type="button" onClick={() => this.props.onClick()}>
          <img src={ searchImg } />
          <span className="glyphicon glyphicon-search" />
        </button>
      </span>
    );
  }
}

SearchButton.defaultProps = {};

export default SearchButton;
