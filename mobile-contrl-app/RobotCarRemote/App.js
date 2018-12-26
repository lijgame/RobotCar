import React from 'react';
import {Animated, Image, PanResponder, StyleSheet, Text, View} from 'react-native';

export default class App extends React.Component {
  constructor(props) {
    super(props);

    this.state = {pan: new Animated.ValueXY()};
  }
  render() {
    let {pan} = this.state;

    let [translateX, translateY] = [pan.x, pan.y];

    let imageStyle = {transform: [{translateX}, {translateY}]};
    return (<View style = {styles.container}>
    <Animated.View style = {imageStyle} {...this._panResponder.panHandlers}>
            <Image source = {require('./assets/button.png')}>
            </Image>
    </Animated.View>
      </View>);
  }

  componentWillMount() {
    this._panResponder = PanResponder.create({
      onMoveShouldSetResponderCapture: () => true,
      onMoveShouldSetPanResponderCapture: () => true,
      onPanResponderGrant: (e, gestureState) => {
        this.state.pan.setOffset({x:this.state.pan.x._value,y:this.state.pan.y._value})
        this.state.pan.setValue({x:0,y:0});
      },
      onPanResponderMove: Animated.event([null,{dx:this.state.pan.x, dy: this.state.pan.y}]),
      onPanResponderRelease: (e, {vx, vy}) => {
        this.state.pan.setValue({x:0,y:0});
        this.state.pan.flattenOffset();
  }
})
}
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
    alignItems: 'center',
    justifyContent: 'center',
  },
});
