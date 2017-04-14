import 'core-js/fn/object/assign';
import React from 'react';
import ReactDOM from 'react-dom';

import { Router, Route, hashHistory } from 'react-router'

import Main from './components/Main';
import TestImage from './components/TestImage';

ReactDOM.render(
    <Router history={hashHistory}>
      <Route path="/" component={ Main }/>
      <Route path="/testImage" component={ TestImage }/>
    </Router>
, document.getElementById('app'));
