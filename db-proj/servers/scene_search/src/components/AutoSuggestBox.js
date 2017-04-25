/**
 * Created by erb2 on 2/7/17.
 */

import React, { Component } from 'react';
import Autosuggest from 'react-autosuggest'

import 'whatwg-fetch'

var shuffle = require('shuffle-array')

class AutoSuggestBox extends Component {
  constructor(props) {
    super(props);
    this.handleChange = this.handleChange.bind(this);
    this.handleKeyPress = this.handleKeyPress.bind(this);

    this.onSuggestionSelected = this.onSuggestionSelected.bind(this);
    this.renderInputComponent = this.renderInputComponent.bind(this);
    this.getSuggestionValue = this.getSuggestionValue.bind(this)
    this.shouldRenderSuggestions = this.shouldRenderSuggestions.bind(this);

    this.state = {
      suggestions: [],
      isSelecting: false,
      skipEnter: false
    };
  }

  // Use your imagination to render suggestions.
  renderSuggestion = suggestion => (
    <div>
      {suggestion}
    </div>
  );

  renderSectionTitle(section) {
    return (
      <strong>{section.title}</strong>
    );
  }

  getSectionSuggestions(section) {
    return section.suggestions;
  }

  renderInputComponent = inputProps => (
    <input
      id="query" type="text" className="search-query form-control" placeholder="Search"
      onKeyPress={this.handleKeyPress}
      autoFocus
      {...inputProps}
    />
);

  handleChange() {
    this.props.handleChange(this.textInput.value);
  }

  onChange = (event, { newValue }) => {
    this.props.handleChange(newValue);
  };

  // Autosuggest will call this function every time you need to update suggestions.
  onSuggestionsFetchRequested = ({ value }) => {
    var self = this;
    
    let queryParams = value.split(' ').join('+');
    if (queryParams[queryParams.length-1] === '+')
      queryParams = queryParams.slice(0, queryParams.length-1)
    let url = 'http://localhost:4000/autocomplete?query=' + queryParams;
    
    fetch(url).then(function(response){
      return response.json();
    }).then(function(json){

      Object.keys(json.suggestions).forEach(function(key) {
        if (json.suggestions[key].suggestions.length > 4)
          json.suggestions[key].suggestions = shuffle(json.suggestions[key].suggestions, {'copy': true}).slice(0, 4);
      });

      self.setState({
        suggestions: json.suggestions
      });
    })
  };

  // Autosuggest will call this function every time you need to clear suggestions.
  onSuggestionsClearRequested = () => {
    this.setState({
      suggestions: []
    });
  };

  shouldRenderSuggestions(value) {
    return value[value.length-1] === ' ' || this.state.isSelecting
  }

  handleKeyPress(target) {
    if (target.charCode === 13) {
      if (this.state.skipEnter)
        this.setState({skipEnter: false})
      else
        this.props.onClick()
    } else if (target.charCode === 32) {
      this.setState({
        isSelecting: false
      })
    }
  }

  onSuggestionSelected(event, { suggestion, method }) {
    if (method === 'enter')
      this.setState({ skipEnter: true })
    else
      this.props.handleChange(this.props.query + suggestion)
    
    this.setState({
      isSelecting: false
    })
  }

  getSuggestionValue(suggestion) {
    let value = this.props.query;

    // If not the first word
    if (this.props.query.trim().includes(' ') && this.state.isSelecting) {
      let withoutTrailingSpace = this.props.query.slice(0, this.props.query.length - 1)
      value = this.props.query.slice(0, withoutTrailingSpace.lastIndexOf(' ') + 1)
    }

    this.setState({
      isSelecting: true
    })

    return value + suggestion;
  }

  render() {
    let suggestions = this.state.suggestions;

    // Autosuggest will pass through all these props to the input element.
    const inputProps = {
      placeholder: 'e.g. living room with sofa',
      value: this.props.query,
      onChange: this.onChange
    };

    return (
      <Autosuggest
        suggestions={suggestions}
        onSuggestionsFetchRequested={this.onSuggestionsFetchRequested}
        onSuggestionsClearRequested={this.onSuggestionsClearRequested}
        getSuggestionValue={this.getSuggestionValue}
        getSectionSuggestions={this.getSectionSuggestions}
        renderSuggestion={this.renderSuggestion}
        renderSectionTitle={this.renderSectionTitle}
        shouldRenderSuggestions={this.shouldRenderSuggestions}
        onSuggestionSelected={this.onSuggestionSelected}
        multiSection={true}
        inputProps={inputProps}
        renderInputComponent={this.renderInputComponent}
      />
    );
  }
}

export default AutoSuggestBox;

