import 'core-js/fn/object/assign';
import React from 'react';
import ReactDOM from 'react-dom';

import { Router, Route, hashHistory } from 'react-router'

import Main from './components/Main';
import Extra from './components/Extra';

ReactDOM.render(
    <Router history={hashHistory}>
      <Route path="/" component={ Main }/>
      <Route path="/test" component={ Extra }/>
    </Router>
, document.getElementById('app'));
