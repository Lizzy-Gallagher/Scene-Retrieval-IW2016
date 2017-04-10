/**
 * Created by erb2 on 2/7/17.
 */

require('normalize.css/normalize.css');
require('styles/App.css');

import React, { Component } from 'react';

let yeomanImage = require('../images/yeoman.png');

class Extra extends Component {
  render() {
    return (
      <div className="index">
        <img src={yeomanImage} alt="Yeoman Generator" />
        <div className="notice">Routing works!</div>
      </div>
    );
  }
}

Extra.defaultProps = {
};

export default Extra;

